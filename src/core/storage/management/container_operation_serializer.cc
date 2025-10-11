// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_operation_serializer.cc'
// Author: jcjuarez
// Description:
//      Manages object container operations
//      in a serialized manner. 
// ****************************************************

#include <spdlog/spdlog.h>
#include "../io/storage_engine.hh"
#include "../index/container_index.hh"
#include "../../network/server/server.hh"
#include "../../common/uuid_utilities.hh"
#include "container_operation_serializer.hh"

namespace lazarus
{
namespace storage
{

container_operation_serializer::container_operation_serializer(
    std::shared_ptr<storage_engine_interface> storage_engine_handle,
    std::shared_ptr<container_index> container_index)
    : storage_engine_{std::move(storage_engine_handle)},
      container_index_{std::move(container_index)}
{}

void
container_operation_serializer::enqueue_container_operation(
    schemas::container_request&& container_request,
    network::server_response_callback&& response_callback)
{
    //
    // Enqueue the async object container operation action.
    // This is a single-threaded execution for serialization purposes.
    //
    container_operations_serializer_.enqueue_serialized_task(
        [this,
        container_request = std::move(container_request),
        response_callback = std::move(response_callback)]()
        {
            this->container_operation_serial_proxy(
                container_request,
                response_callback);
        });
}

void
container_operation_serializer::container_operation_serial_proxy(
    const schemas::container_request& container_request,
    const network::server_response_callback& response_callback)
{
    spdlog::info("Executing serialized object container operation action. "
        "OpType={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(container_request.get_optype()),
        container_request.get_name());

    status::status_code status = status::success;

    switch (container_request.get_optype())
    {
        case schemas::container_request_optype::create:
        {
            status = handle_container_creation(
                container_request);
            break;
        }
        case schemas::container_request_optype::remove:
        {
            status = handle_container_removal(
                container_request);
            break;
        }
        default:
        {
            status = status::invalid_operation;
            break;
        }
    }

    //
    // Provide the response back to the client over the async callback.
    //
    network::server::send_response(
        response_callback,
        status);
}

status::status_code
container_operation_serializer::handle_container_creation(
    const schemas::container_request& container_request)
{
    status::status_code status =
        container_index_->get_container_existence_status(
            container_request.get_name());

    if (status != status::container_not_exists)
    {
        //
        // At this point, it is guaranteed that this operation is serialized
        // and that the object container is in a non-creatable state; no further action needed.
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

    //
    // Object container does not exist at this point.
    // Given the storage engine does not support atomic object container creations,
    // create first the object container and then register the new metadata for it.
    // In case the data store crashes in between, the object container will be orphaned
    // and the garbage collector will be in charge to clean it up.
    //
    storage_engine_reference_handle* container_storage_engine_reference;
    status = storage_engine_->create_container(
        container_request.get_name().c_str(),
        &container_storage_engine_reference);

    if (status::failed(status))
    {
        spdlog::error("Storage engine failed to create the new object container. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    //
    // Insert the metadata for the newly created object container to the storage engine.
    //
    const schemas::container_persistent_interface container_persistent_metadata =
        container::create_container_persistent_metadata(container_request.get_name().c_str());
    byte_stream serialized_container_persistent_metadata;
    container_persistent_metadata.SerializeToString(&serialized_container_persistent_metadata);
    status = storage_engine_->insert_object(
        container_index_->get_containers_internal_metadata_storage_engine_reference(),
        container_request.get_name().c_str(),
        serialized_container_persistent_metadata);

    if (status::failed(status))
    {
        spdlog::error("Storage engine failed insert the metadata entry for the new object container. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    //
    // Index the new object container to the internal metadata table.
    //
    status = container_index_->insert_container(
        container_storage_engine_reference,
        container_persistent_metadata);

    if (status::failed(status))
    {
        spdlog::error("Container index insertion failed for the new object container. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    spdlog::info("Object container creation succeeded. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(container_request.get_optype()),
        container_request.get_name());

    return status::success;
}

status::status_code
container_operation_serializer::handle_container_removal(
    const schemas::container_request& container_request)
{
    status::status_code status =
        container_index_->get_container_existence_status(
            container_request.get_name());

    if (status != status::container_already_exists)
    {
        //
        // At this point, it is guaranteed that this operation is serialized
        // and that the object container is in a non-deletable state; no further action needed.
        //
        spdlog::error("Object container creation will be failed as the "
            "object container is in a non-deletable state. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status::container_not_exists;
    }

    //
    // First, remove the object container from the filesystem internal metadata.
    // After this operation is completed, the link between the metadata and the actual
    // object container will be broken, and only the in-memory object container reference
    // will remain active for the rest of this session and be cleaned up by the garbage collector.
    //
    status = storage_engine_->remove_object(
        container_index_->get_containers_internal_metadata_storage_engine_reference(),
        container_request.get_name().c_str());

    if (status::failed(status))
    {
        spdlog::error("Failed to remove object container from the internal filesystem metadata. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    //
    // At this point, it is guaranteed that the object container has been removed
    // from the internal filesystem metadata. Now, mark the remaining in-memory index table
    // metadata as deleted for later garbage collector cleanup. In case a crash occurs in between, the
    // filesystem object container will be detected as orphaned during startup and will also be cleaned up.
    //
    std::shared_ptr<container> container =
        container_index_->get_container(
            container_request.get_name());

    if (container == nullptr)
    {
        //
        // This should never happen.
        // This indicates a resource leak for the lifetime of the session.
        //
        spdlog::error("Failed to mark the object container as deleted. "
            "Resource will be leaked for the lifetime of the current session. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status::container_not_exists;
    }

    //
    // Marking it as deleted.
    //
    container->mark_as_deleted();

    spdlog::info("Object container internal metadata deletion marking succeeded. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(container_request.get_optype()),
        container_request.get_name());

    return status::success;
}

} // namespace storage.
} // namespace lazarus.