// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container_operation_serializer.cc'
// Author: jcjuarez
// Description:
//      Manages object container operations
//      in a serialized manner. 
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "object_container_index.hh"
#include "../network/server/server.hh"
#include "../common/uuid_utilities.hh"
#include "object_container_operation_serializer.hh"

namespace lazarus
{
namespace storage
{

object_container_operation_serializer::object_container_operation_serializer(
    std::shared_ptr<storage_engine> storage_engine_handle,
    std::shared_ptr<object_container_index> object_cotainer_index_handle)
    : storage_engine_{std::move(storage_engine_handle)},
      object_container_index_{std::move(object_cotainer_index_handle)},
      serializer_queue_{1u} // Must always be a single-threaded task queue.
{}

void
object_container_operation_serializer::enqueue_object_container_operation(
    schemas::object_container_request_interface&& object_container_request,
    network::server_response_callback&& response_callback)
{
    //
    // Enqueue the async object container operation action.
    // This is a single-threaded execution for serialization purposes.
    //
    serializer_queue_.execute(
        [this,
        object_container_request = std::move(object_container_request),
        response_callback = std::move(response_callback)]() mutable
        {
            this->object_container_operation_serial_proxy(
                std::move(object_container_request),
                std::move(response_callback));
        });
}

void
object_container_operation_serializer::object_container_operation_serial_proxy(
    const schemas::object_container_request_interface&& object_container_request,
    network::server_response_callback&& response_callback)
{
    spdlog::info("Executing serialized object container operation action. "
        "OpType={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_container_request.get_optype()),
        object_container_request.get_name());

    status::status_code status;

    switch (object_container_request.get_optype())
    {
        case schemas::object_container_request_optype::create:
        {
            status = handle_object_container_creation(
                object_container_request);
            break;
        }
        case schemas::object_container_request_optype::remove:
        {
            status = handle_object_container_removal(
                object_container_request);
            break;
        }
        default:
        {
            status = status::invalid_operation;
            break;
        }
    }

    network::server::send_response(
        response_callback,
        status);
}

status::status_code
object_container_operation_serializer::handle_object_container_creation(
    const schemas::object_container_request_interface& object_container_request)
{
    status::status_code status =
        object_container_index_->get_object_container_existence_status(
            object_container_request.get_name());

    if (status != status::object_container_not_exists)
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
            static_cast<std::uint8_t>(object_container_request.get_optype()),
            object_container_request.get_name(),
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
    storage_engine_reference_handle* object_container_storage_engine_reference;
    status = storage_engine_->create_object_container(
        object_container_request.get_name(),
        &object_container_storage_engine_reference);

    if (status::failed(status))
    {
        spdlog::error("Storage engine failed to create the new object container. "
            "Optype={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_container_request.get_optype()),
            object_container_request.get_name());

        return status;
    }

    //
    // Insert the metadata for the newly created object container to the storage engine.
    //
    const schemas::object_container_persistent_interface object_container_persistent_metadata =
        object_container::create_object_container_persistent_metadata(object_container_request.get_name());
    byte_stream serialized_object_container_persistent_metadata;
    object_container_persistent_metadata.SerializeToString(&serialized_object_container_persistent_metadata);
    status = storage_engine_->insert_object(
        object_container_index_->get_object_containers_internal_metadata_storage_engine_reference(),
        object_container_request.get_name(),
        serialized_object_container_persistent_metadata.c_str());

    if (status::failed(status))
    {
        spdlog::error("Storage engine failed insert the metadata entry for the new object container. "
            "Optype={}, "
            "ObjectContainerName={}.",
            static_cast<std::uint8_t>(object_container_request.get_optype()),
            object_container_request.get_name());

        return status;
    }

    //
    // Index the new object container to the internal metadata table.
    //
    object_container_index_->insert_object_container(
        object_container_storage_engine_reference,
        object_container_persistent_metadata);

    spdlog::info("Object container creation succeeded. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_container_request.get_optype()),
        object_container_request.get_name());

    return status::success;
}

status::status_code
object_container_operation_serializer::handle_object_container_removal(
    const schemas::object_container_request_interface& object_container_request)
{
    status::status_code status =
        object_container_index_->get_object_container_existence_status(
            object_container_request.get_name());

    if (status != status::object_container_already_exists)
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
            static_cast<std::uint8_t>(object_container_request.get_optype()),
            object_container_request.get_name(),
            status);

        return status::object_container_not_exists;
    }

    //
    // First, remove the object container from the filesystem internal metadata.
    // After this operation is completed, the link between the metadata and the actual
    // object container will be broken, and only the in-memory object container reference
    // will remain active for the rest of this session and be cleaned up by the garbage collector.
    //
    status = storage_engine_->remove_object(
        object_container_index_->get_object_containers_internal_metadata_storage_engine_reference(),
        object_container_request.get_name());

    if (status::failed(status))
    {
        spdlog::error("Failed to remove object container from the internal filesystem metadata. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_container_request.get_optype()),
            object_container_request.get_name(),
            status);

        return status;
    }

    //
    // At this point, it is guaranteed that the object container has been removed
    // from the internal filesystem metadata. Now, mark the remaining in-memory index table
    // metadata as deleted for later garbage collector cleanup. In case a crash occurs in between, the
    // filesystem object container will be detected as orphaned during startup and will also be cleaned up.
    //
    std::shared_ptr<object_container> object_container =
        object_container_index_->get_object_container(
            object_container_request.get_name());

    if (object_container == nullptr)
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
            static_cast<std::uint8_t>(object_container_request.get_optype()),
            object_container_request.get_name(),
            status);

        return status::object_container_not_exists;
    }

    //
    // Marking it as deleted.
    //
    object_container->mark_as_deleted();

    spdlog::info("Object container internal metadata deletion marking succeeded. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_container_request.get_optype()),
        object_container_request.get_name());

    return status::success;
}

} // namespace storage.
} // namespace lazarus.