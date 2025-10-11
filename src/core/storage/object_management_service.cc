// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'object_management_service.cc'
// Author: jcjuarez
// Description:
//      Management service for object operations.
// ****************************************************

#include <spdlog/spdlog.h>
#include "container_index.hh"
#include "frontline_cache.hh"
#include "read_io_dispatcher.hh"
#include "write_io_dispatcher.hh"
#include "object_management_service.hh"
#include "../common/request_validations.hh"

namespace lazarus
{
namespace storage
{

object_management_service::object_management_service(
    const storage_configuration& storage_configuration,
    std::shared_ptr<container_index> container_index,
    std::shared_ptr<io_dispatcher_interface> write_request_dispatcher,
    std::shared_ptr<io_dispatcher_interface> read_request_dispatcher,
    std::shared_ptr<storage::frontline_cache> frontline_cache)
    : storage_configuration_{storage_configuration},
      container_index_{std::move(container_index)},
      write_io_task_dispatcher_{std::move(write_request_dispatcher)},
      read_io_task_dispatcher_{std::move(read_request_dispatcher)},
      frontline_cache_{std::move(frontline_cache)}
{}

status::status_code
object_management_service::validate_object_operation_request(
    const schemas::object_request& object_request)
{
    status::status_code status = validate_request_parameters(object_request);

    if (status::failed(status))
    {
        //
        // Parameters validations failed.
        //
        return status;
    }

    //
    // Only log the request parameters after
    // they have been validated to be well-formed.
    //
    if (!is_object_operation_optype_valid(object_request.get_optype()))
    {
        spdlog::error("Invalid optype received for object operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name());

        return status::invalid_operation;
    }

    return status::success;
}

status::status_code
object_management_service::validate_request_parameters(
    const schemas::object_request& object_request)
{
    status::status_code status = common::request_validations::validate_container_name(
        object_request.get_container_name(),
        storage_configuration_);

    if (status::failed(status))
    {
        //
        // Given object container name is invalid.
        // Not logging the parameters as to avoid potential
        // large-buffer attacks in case the parameters are too big.
        //
        spdlog::error("Object operation will be failed as the "
            "object container name is invalid. "
            "Optype={}, "
            "ObjectContainerNameSizeInBytes={}, "
            "ObjectContainerNameMaxSizeInBytes={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_container_name().size(),
            storage_configuration_.max_container_name_size_bytes_,
            status);

        return status;
    }

    status = common::request_validations::validate_object_id(
        object_request.get_object_id(),
        storage_configuration_);

    if (status::failed(status))
    {
        //
        // Given object ID is invalid.
        // Not logging the parameters as to avoid potential
        // large-buffer attacks in case the parameters are too big.
        //
        spdlog::error("Object operation will be failed as the "
            "object ID is invalid. "
            "Optype={}, "
            "ObjectIdSizeInBytes={}, "
            "ObjectIdMaxSizeInBytes={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id().size(),
            storage_configuration_.max_object_id_size_bytes_,
            status);

        return status;
    }

    if (object_request.get_optype() == schemas::object_request_optype::insert)
    {
        status = common::request_validations::validate_object_data(
            object_request.get_object_data(),
            storage_configuration_);

        if (status::failed(status))
        {
            //
            // Given object data stream is invalid.
            // Not logging the parameters as to avoid potential
            // large-buffer attacks in case the parameters are too big.
            //
            spdlog::error("Object operation will be failed as the "
                "object data stream is invalid. "
                "Optype={}, "
                "ObjectDataStreamSizeInBytes={}, "
                "ObjectDataStreamMaxSizeInBytes={}, "
                "Status={:#x}.",
                static_cast<std::uint8_t>(object_request.get_optype()),
                object_request.get_object_data().size(),
                storage_configuration_.max_object_data_size_bytes_,
                status);

            return status;
        }
    }

    return status::success;
}

std::shared_ptr<container>
object_management_service::get_container_reference(
    const std::string& container_name)
{
    return container_index_->get_container(container_name);
}

status::status_code
object_management_service::orchestrate_concurrent_write_request(
    schemas::object_request&& object_request,
    std::shared_ptr<container> container,
    network::server_response_callback&& response_callback)
{
    //
    // Sanity check for the operation type before
    // delegating the task to the thread pool.
    //
    if (!is_object_request_write_io_operation(object_request.get_optype()))
    {
        spdlog::error("Invalid write request optype for object operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name());

        return status::invalid_operation;
    }

    //
    // Create the long-lived write IO task to be dispatched down to the storage engine.
    //
    object_io_task io_task {
        std::move(object_request),
        std::move(container),
        std::move(response_callback)};

    write_io_task_dispatcher_->enqueue_io_task(
        std::move(io_task));

    return status::success;
}

status::status_code
object_management_service::orchestrate_concurrent_read_request(
    schemas::object_request&& object_request,
    std::shared_ptr<container> container,
    network::server_response_callback&& response_callback)
{
    //
    // Sanity check for the operation type before
    // delegating the task to the thread pool.
    //
    if (!is_object_request_read_io_operation(object_request.get_optype()))
    {
        spdlog::error("Invalid read request optype for object operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name());

        return status::invalid_operation;
    }

    //
    // Create the long-lived read IO task to be dispatched down to the storage engine.
    //
    object_io_task io_task {
        std::move(object_request),
        std::move(container),
        std::move(response_callback)};

    read_io_task_dispatcher_->enqueue_io_task(
        std::move(io_task));

    return status::success;
}

std::optional<byte_stream>
object_management_service::get_object_from_frontline_cache(
    const std::string& object_id,
    const std::string& container_name)
{
    return frontline_cache_->get(
        object_id,
        container_name);
}

bool
object_management_service::is_object_operation_optype_valid(
    const schemas::object_request_optype optype)
{
    return optype == schemas::object_request_optype::insert ||
           optype == schemas::object_request_optype::get ||
           optype == schemas::object_request_optype::remove;
}

bool
object_management_service::is_object_request_write_io_operation(
    schemas::object_request_optype optype)
{
    return optype == schemas::object_request_optype::insert ||
           optype == schemas::object_request_optype::remove;
}

bool
object_management_service::is_object_request_read_io_operation(
    schemas::object_request_optype optype)
{
    return optype == schemas::object_request_optype::get;
}

} // namespace storage.
} // namespace lazarus.