// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_management_service.cc'
// Author: jcjuarez
// Description:
//      Management service for object container
//      operations.
// ****************************************************

#include <rocksdb/db.h>
#include <spdlog/spdlog.h>
#include "../io/data_partition.hh"
#include "../gc/garbage_collector.hh"
#include "../index/container_index.hh"
#include "container_management_service.hh"
#include "../io/data_partition_provider.hh"
#include "container_operation_serializer.hh"
#include "container_persistent_interface.pb.h"
#include "../../common/request_validations.hh"

namespace lazarus
{
namespace storage
{

container_management_service::container_management_service(
    const storage_configuration& storage_configuration,
    data_partition& container_metadata_partition,
    container_index& container_index_handle,
    std::unique_ptr<container_operation_serializer> container_operation_serializer_handle,
    data_partition_provider& data_partition_provider)
    : storage_configuration_{storage_configuration},
      container_metadata_partition_{container_metadata_partition},
      container_index_{container_index_handle},
      container_operation_serializer_{std::move(container_operation_serializer_handle)},
      data_partition_provider_{data_partition_provider}
{}

void
container_management_service::orchestrate_serial_container_operation(
    schemas::container_request&& container_request,
    network::server_response_callback&& response_callback)
{
    container_operation_serializer_->enqueue_container_operation(
        std::move(container_request),
        std::move(response_callback));
}

status::status_code
container_management_service::validate_container_operation_request(
    const schemas::container_request& container_request)
{
    status::status_code status = common::request_validations::validate_container_name(
        container_request.get_name(),
        storage_configuration_);

    if (status::failed(status))
    {
        //
        // Given object container name is invalid.
        // Not logging the container name as to avoid potential
        // large-buffer attacks in case the name is too big.
        //
        spdlog::error("Object container operation will be failed as the "
            "object container name is invalid. "
            "Optype={}, "
            "ObjectContainerNameSizeInBytes={}, "
            "ObjectContainerNameMaxSizeInBytes={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name().size(),
            storage_configuration_.max_container_name_size_bytes_,
            status);

        return status;
    }

    //
    // Only log the request parameters after
    // they have been validated to be well-formed.
    //
    switch (container_request.get_optype())
    {
        case schemas::container_request_optype::create:
        {
            return validate_container_create_request(container_request);
            break;
        }
        case schemas::container_request_optype::remove:
        {
            return validate_container_remove_request(container_request);
            break;
        }
        default:
        {
            spdlog::error("Invalid optype received for container operation. "
                "Optype={}, "
                "ObjectContainerName={}.",
                static_cast<std::uint8_t>(container_request.get_optype()),
                container_request.get_name());

            return status::invalid_operation;
            break;
        }
    }

    return status::unreachable;
}

status::status_code
container_management_service::validate_container_create_request(
    const schemas::container_request& container_request)
{
    const status::status_code status =
        container_index_.get_container_existence_status(
            container_request.get_name());

    if (status != status::container_not_exists)
    {
        //
        // Fail fast in case the object container already exists.
        //
        spdlog::error("Object container creation will be failed as the "
            "object container is in a non-creatable state. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    if (container_index_.get_total_number_containers() >=
        storage_configuration_.max_number_containers_)
    {
        //
        // The total number of object containers present on the
        // system exceeds the limit. Fail the new creation operation.
        //
        spdlog::error("Object container creation will be failed as the "
            "current number of object containers exceeds the limit. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "TotalNumberOfObjectContainers={}, "
            "MaxNumberOfObjectContainers={}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            container_index_.get_total_number_containers(),
            storage_configuration_.max_number_containers_);

        return status::max_number_containers_reached;
    }

    return status::success;
}

status::status_code
container_management_service::validate_container_remove_request(
    const schemas::container_request& container_request)
{
    const status::status_code status =
        container_index_.get_container_existence_status(
            container_request.get_name());

    if (status != status::container_already_exists)
    {
        //
        // Fail fast in case the object container does not exist.
        //
        spdlog::error("Object container removal will be failed as the "
            "object container is in a non-deletable state. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    return status::success;
}

} // namespace storage.
} // namespace lazarus.