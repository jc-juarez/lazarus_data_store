// ****************************************************
// Lazarus Data Store
// Storage
// 'object_management_service.cc'
// Author: jcjuarez
// Description:
//      Management service for object operations.
// ****************************************************

#include <spdlog/spdlog.h>
#include "object_container_index.hh"
#include "write_io_dispatcher.hh"
#include "object_management_service.hh"
#include "../common/request_validations.hh"

namespace lazarus
{
namespace storage
{

object_management_service::object_management_service(
    const storage_configuration& storage_configuration,
    std::shared_ptr<object_container_index> object_container_index,
    std::shared_ptr<write_io_dispatcher> write_request_dispatcher)
    : storage_configuration_{storage_configuration},
      object_container_index_{std::move(object_container_index)},
      write_request_dispatcher_{std::move(write_request_dispatcher)}
{}

status::status_code
object_management_service::validate_object_operation_request(
    const schemas::object_request_interface& object_request)
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
            object_request.get_object_container_name());

        return status::invalid_operation;
    }

    return status::success;
}

status::status_code
object_management_service::validate_request_parameters(
    const schemas::object_request_interface& object_request)
{
    status::status_code status = common::request_validations::validate_object_container_name(
        object_request.get_object_container_name(),
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
            object_request.get_object_container_name().size(),
            storage_configuration_.max_object_container_name_size_bytes_,
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

    return status::success;
}

std::shared_ptr<object_container>
object_management_service::get_object_container_reference(
    const std::string& object_container_name)
{
    return object_container_index_->get_object_container(object_container_name.c_str());
}

status::status_code
object_management_service::orchestrate_concurrent_write_request(
    schemas::object_request_interface&& object_request,
    std::shared_ptr<object_container> object_container,
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
            object_request.get_object_container_name());

        return status::invalid_operation;
    }

    write_request_dispatcher_->enqueue_concurrent_io_request(
        std::move(object_request),
        object_container,
        std::move(response_callback));

    return status::success;
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

} // namespace storage.
} // namespace lazarus.