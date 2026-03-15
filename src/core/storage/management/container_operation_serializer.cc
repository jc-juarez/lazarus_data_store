// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'container_operation_serializer.cc'
// Author: jcjuarez
// Description:
//      Manages object container operations
//      in a serialized manner. 
// ****************************************************

#include "../io/storage_engine.hh"
#include "../io/data_partition.hh"
#include "../index/container_index.hh"
#include "../../network/server/server.hh"
#include "../io/data_partition_provider.hh"
#include "container_operation_serializer.hh"

namespace pandora
{
namespace storage
{

container_operation_serializer::container_operation_serializer(
    data_partition& metadata_partition,
    data_partition_provider& data_partition_provider,
    container_index& container_index)
    : metadata_partition_{metadata_partition},
      data_partition_provider_{data_partition_provider},
      container_index_{container_index}
{
    //
    // Name the serializer thread.
    //
    container_operations_serializer_.enqueue_serialized_task(
        []()
        {
            pthread_setname_np(pthread_self(), "pandora_serial");
        });
}

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
    TRACE_LOG(info, "Executing serialized object container operation action. "
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
        container_index_.get_container_existence_status(
            container_request.get_name());

    if (status != status::container_not_exists)
    {
        //
        // At this point, it is guaranteed that this operation is serialized
        // and that the object container is in a non-creatable state; no further action needed.
        //
        TRACE_LOG(error, "Object container creation will be failed as the "
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
    // create first the object container across all data partitions and then register
    // the new metadata for it. In case the data store crashes in between, the object
    // container will be orphaned and the garbage collector will be in charge to clean it up.
    //
    auto container_instances_creation_result = create_container_instances_on_data_partitions(
        container_request.get_name());

    if (!container_instances_creation_result)
    {
        TRACE_LOG(error, "Object container creation across data partitions failed. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            container_instances_creation_result.error());

        return container_instances_creation_result.error();
    }

    //
    // Insert the metadata for the newly created object container to the storage engine.
    //
    const schemas::container_persistent_interface container_persistent_metadata =
        container::create_container_persistent_metadata(container_request.get_name().c_str());
    byte_stream serialized_container_persistent_metadata;
    container_persistent_metadata.SerializeToString(&serialized_container_persistent_metadata);
    status = metadata_partition_.get_storage_engine().insert_object(
        container_index_.get_container_metadata_engine_reference(),
        container_request.get_name().c_str(),
        serialized_container_persistent_metadata);

    if (status::failed(status))
    {
        TRACE_LOG(error, "Storage engine failed insert the metadata entry for the new object container. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    //
    // At this point, a WDT (Well Defined Transaction) has been completed
    // for the container creation, so it is safe to mark its engine references as approved.
    //
    const std::vector<container_instance> container_instances = container_instances_creation_result.value();
    mark_engine_references_as_approved(container_instances);

    //
    // Index the new object container to the internal metadata table
    // after all engine references have been made publicly available.
    //
    status = container_index_.insert_container(
        container_persistent_metadata,
        container_instances);

    if (status::failed(status))
    {
        TRACE_LOG(error, "Container index insertion failed for the new object container. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(container_request.get_optype()),
            container_request.get_name(),
            status);

        return status;
    }

    TRACE_LOG(info, "Object container creation succeeded. "
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
        container_index_.get_container_existence_status(
            container_request.get_name());

    if (status != status::container_already_exists)
    {
        //
        // At this point, it is guaranteed that this operation is serialized
        // and that the object container is in a non-deletable state; no further action needed.
        //
        TRACE_LOG(error, "Object container creation will be failed as the "
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
    status = metadata_partition_.get_storage_engine().remove_object(
        container_index_.get_container_metadata_engine_reference(),
        container_request.get_name().c_str());

    if (status::failed(status))
    {
        TRACE_LOG(error, "Failed to remove object container from the internal filesystem metadata. "
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
        container_index_.get_container(
            container_request.get_name());

    if (container == nullptr)
    {
        //
        // This should never happen.
        // This indicates a resource leak for the lifetime of the session.
        //
        TRACE_LOG(error, "Failed to mark the object container as deleted. "
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

    TRACE_LOG(info, "Object container internal metadata deletion marking succeeded. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(container_request.get_optype()),
        container_request.get_name());

    return status::success;
}

std::expected<
    std::vector<container_instance>,
    status::status_code>
container_operation_serializer::create_container_instances_on_data_partitions(
    const std::string& container_name)
{
    std::vector<container_instance> container_instances;

    const std::span<data_partition> data_partitions =
        data_partition_provider_.get_all_partitions();

    for (auto& data_partition : data_partitions)
    {
        storage_engine_reference* container_storage_engine_reference;

        status::status_code status = data_partition.get_storage_engine().create_container(
            container_name.c_str(),
            &container_storage_engine_reference);

        if (status::failed(status))
        {
            TRACE_LOG(error, "Failed to create container on DataPartitionCollocationIndex={}. "
                "ObjectContainerName={}, "
                "Status={:#x}.",
                data_partition.get_collocation_index(),
                container_name,
                status);

            return std::unexpected(status);
        }

        container_instances.emplace_back(
            data_partition.get_collocation_index(),
            data_partition.get_storage_engine(),
            container_storage_engine_reference);
    }

    return container_instances;
}

void
container_operation_serializer::mark_engine_references_as_approved(
    const std::vector<container_instance>& container_instances)
{
    //
    // The instances vector is 0-based for the respective data collocations.
    //
    for (std::uint16_t collocation_index = 0u; collocation_index < container_instances.size(); ++collocation_index)
    {
        storage_engine_interface& engine =
            data_partition_provider_.get_partition_by_collocation(collocation_index).get_storage_engine();

        engine.register_approved_engine_references(
            {container_instances.at(collocation_index).storage_engine_reference_});
    }
}

} // namespace storage.
} // namespace pandora.