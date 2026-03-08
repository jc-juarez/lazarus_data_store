// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
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
    std::shared_ptr<data_partition> container_metadata_partition,
    std::shared_ptr<container_index> container_index_handle,
    std::unique_ptr<container_operation_serializer> container_operation_serializer_handle,
    std::shared_ptr<storage::data_partition_provider> data_partition_provider)
    : storage_configuration_{storage_configuration},
      container_metadata_partition_{std::move(container_metadata_partition)},
      container_index_{std::move(container_index_handle)},
      container_operation_serializer_{std::move(container_operation_serializer_handle)},
      data_partition_provider_{std::move(data_partition_provider)}
{}

status::status_code
container_management_service::populate_container_index(
    std::unordered_map<std::string, storage_engine_reference_handle*>& container_metadata_partition_references,
    container_reference_registry& structured_partitions_registry)
{
    //
    // Keep track of the internal metadata object containers.
    // The only case when it is valid for them to be absent is when the column
    // family references mapping only has 1 entry (kDefaultColumnFamilyName), which
    // implies that the data store is starting up for the first time.
    //
    if (container_metadata_partition_references.size() == 1u)
    {
        //
        // This means this is the first ever startup for the data store,
        // or that it simply does not need a previous metadata state for working.
        // Create all required internal metadata root object containers.
        //
        create_container_metadata_column_family(&container_metadata_partition_references);
    }

    //
    // Ensure the container metadata container is present; if not, this is a
    // critical error as the data store cannot function without this critical metadata.
    //
    if (container_metadata_partition_references.find(container_index::k_container_metadata_name) ==
        container_metadata_partition_references.end())
    {
        //
        // This is a critical error as the persistent container metadata to discover
        // previously existing containers is not present. Fail the system startup.
        //
        spdlog::critical("Failed to find find the storage engine reference for the '{}' internal metadata.",
            container_index::k_container_metadata_name);

        return status::containers_internal_metadata_lookup_failed;
    }

    storage_engine_reference_handle* container_metadata_engine_reference =
        container_metadata_partition_references.at(container_index::k_container_metadata_name);

    //
    // Get all known object containers to the system from the
    // persistent container metadata and populate the rest of the object container index.
    //
    std::unordered_map<std::string, byte_stream> containers_present_on_metadata;
    status::status_code status = container_metadata_partition_->get_storage_engine().get_all_objects_from_container(
        container_metadata_engine_reference,
        &containers_present_on_metadata);

    if (status::failed(status))
    {
        spdlog::critical("Failed to get all container names from the container metadata partition. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // Index all container metadata containers.
    //
    status = index_containers_from_container_metadata_partition(
        container_metadata_partition_references);

    if (status::failed(status))
    {
        spdlog::critical("Failed to index the container metadata internal containers. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // Index all containers known to the persistent
    // container metadata into the container index.
    //
    status = index_structured_partition_containers(
        structured_partitions_registry,
        containers_present_on_metadata);

    if (status::failed(status))
    {
        spdlog::critical("Failed to index the containers known to the persistent container metadata. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // Look for any containers present on the filesystem
    // but not known to the persistent container metadata.
    //
    status = scan_and_index_orphaned_containers(
        structured_partitions_registry,
        containers_present_on_metadata);

    if (status::failed(status))
    {
        spdlog::critical("Failed to scan and index orphaned containers during startup. "
            "Status={:#x}.",
            status);

        return status;
    }

    return status::success;
}

status::status_code
container_management_service::create_container_metadata_column_family(
    std::unordered_map<std::string, storage_engine_reference_handle*>* container_metadata_partition_references)
{
    //
    // Create the object containers column family on the storage engine.
    //
    storage_engine_reference_handle* container_metadata_engine_reference;
    status::status_code status = container_metadata_partition_->get_storage_engine().create_container(
        container_index::k_container_metadata_name,
        &container_metadata_engine_reference);

    if (status::failed(status))
    {
        spdlog::critical("Failed to create internal metadata column family '{}'.",
            container_index::k_container_metadata_name);

        return status;
    }

    //
    // All structured data partitions which thus container tracks will always spin up
    // the default column family, so it should be registered inside this container.
    //
    const schemas::container_persistent_interface container_persistent_metadata =
        container::create_container_persistent_metadata(rocksdb::kDefaultColumnFamilyName.c_str());
    byte_stream serialized_container_persistent_metadata;
    container_persistent_metadata.SerializeToString(&serialized_container_persistent_metadata);
    status = container_metadata_partition_->get_storage_engine().insert_object(
        container_metadata_engine_reference,
        rocksdb::kDefaultColumnFamilyName.c_str(),
        serialized_container_persistent_metadata);

    if (status::failed(status))
    {
        spdlog::critical("Failed to insert default column family for the container metadata container.");

        return status;
    }

    //
    // Append to the storage references mapping for the container metadata partition.
    //
    container_metadata_partition_references->emplace(
        container_index::k_container_metadata_name,
        container_metadata_engine_reference);

    return status::success;
}

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
        container_index_->get_container_existence_status(
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

    if (container_index_->get_total_number_containers() >=
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
            container_index_->get_total_number_containers(),
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
        container_index_->get_container_existence_status(
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

status::status_code
container_management_service::index_containers_from_container_metadata_partition(
    std::unordered_map<std::string, storage_engine_reference_handle*>& container_metadata_partition_references)
{
    for (auto& container_to_index : container_metadata_partition_references)
    {
        //
        // Containers on the metadata partition only live under such partition,
        // so it is only needed to pass down the respective storage engine reference for such partition.
        //
        container_partition_metadata container_metadata {
            container_metadata_partition_->get_collocation_index(),
            container_metadata_partition_->get_storage_engine(),
            container_to_index.second};
        std::vector<container_partition_metadata> container_instances {container_metadata};

        //
        // The default container is a special case in the system given it is present
        // on all data partitions, in all N structured data partitions as well as in the
        // container metadata partition. This will create an internal conflict because the system
        // needs to track to different semantic containers for closing all engine references appropriately:
        //
        // ContainerMetadataPartitionDefault = 1 [EngineReference=0x11223344]
        // StructuredMetadataPartitionDefault = N [EngineReference=0x22334455, EngineReference=0x3345566, ...]
        //
        // If both of these semantic containers are left with the same name as "default", there will be a collision
        // in the name, so we need to have separate names at least at the container index level.
        //
        std::string container_name = container_to_index.first;
        if (container_name == rocksdb::kDefaultColumnFamilyName)
        {
            container_name = k_default_container_name_metadata_partition;
        }

        const schemas::container_persistent_interface container_persistent_metadata =
            container::create_container_persistent_metadata(container_name.c_str());
        status::status_code status = container_index_->insert_container(
            container_persistent_metadata,
            container_instances);

        if (status::failed(status))
        {
            spdlog::critical("Failed to insert container from container metadata into the container index. "
                "ContainerName={}, "
                "Status={:#x}.",
                container_persistent_metadata.name().c_str(),
                status);

            return status;
        }

        const std::shared_ptr<container> container =
            container_index_->get_container(container_name);

        spdlog::info("Indexed internal metadata container on startup. "
            "ContainerMetadata={}.",
            container->to_string());
    }

    return status::success;
}

status::status_code
container_management_service::index_structured_partition_containers(
    container_reference_registry& structured_partitions_registry,
    std::unordered_map<std::string, byte_stream>& containers_present_on_metadata)
{
    for (auto& container_present_on_metadata : containers_present_on_metadata)
    {
        const std::string& container_name = container_present_on_metadata.first;
        const byte_stream& container_raw_metadata = container_present_on_metadata.second;
        const std::optional<std::vector<storage_engine_reference_handle*>> engine_references =
            structured_partitions_registry.get_references(container_name);

        if (engine_references == std::nullopt)
        {
            //
            // This is a critical inconsistency problem.
            // This implies that the persistent container metadata has a record of a container
            // which is not present on the filesystem across the structured data partitions.
            //
            spdlog::critical("Failed to locate a container known to the persistent container metadata "
                "on the filesystem across the structured data partitions. "
                "ContainerName={}.",
                container_name);

            return status::missing_container_on_data_partitions;
        }

        schemas::container_persistent_interface container_persistent_metadata;
        const bool is_parsing_successful = container_persistent_metadata.ParseFromString(container_raw_metadata);
        if (!is_parsing_successful)
        {
            spdlog::critical("Failed to parse a container raw metadata on startup. "
                "ContainerName={}.",
                container_name);

            return status::parsing_failed;
        }

        std::vector<container_partition_metadata> container_instances =
            convert_ordered_engine_references_to_container_instances(engine_references.value());

        status::status_code status = container_index_->insert_container(
            container_persistent_metadata,
            container_instances);

        if (status::failed(status))
        {
            spdlog::critical("Failed to insert container into the container index. "
                "ContainerName={}, "
                "Status={:#x}.",
                container_persistent_metadata.name().c_str(),
                status);

            return status;
        }

        const std::shared_ptr<container> container =
            container_index_->get_container(container_name);

        spdlog::info("Found container on structured data partitions during startup and indexed into "
            "the object containers metadata table. "
            "ContainerMetadata={}.",
            container->to_string());
    }

    return status::success;
}

status::status_code
container_management_service::scan_and_index_orphaned_containers(
    container_reference_registry& structured_partitions_registry,
    std::unordered_map<std::string, byte_stream>& containers_present_on_metadata)
{
    for (const auto& registry_entry : structured_partitions_registry)
    {
        const std::string& container_name = registry_entry.first;
        const std::vector<storage_engine_reference_handle*>& engine_references = registry_entry.second;

        if (containers_present_on_metadata.find(container_name) == containers_present_on_metadata.end())
        {
            //
            // This implies this is an orphaned container present on the structured data partitions on the
            // filesystem, but the persistent container metadata is not aware of it. Mark it as deleted for
            // the garbage collector to clean it up later.
            //
            spdlog::warn("Found orphaned object container on startup to be cleaned up by the garbage collector. "
                "ObjectContainerName={}.",
                container_name.c_str());

            std::vector<container_partition_metadata> container_instances =
                convert_ordered_engine_references_to_container_instances(engine_references);

            const schemas::container_persistent_interface container_persistent_metadata =
                container::create_container_persistent_metadata(container_name.c_str());
            status::status_code status = container_index_->insert_container(
                container_persistent_metadata,
                container_instances);

            if (status::failed(status))
            {
                spdlog::critical("Failed to insert orphaned container into the container index. "
                    "ContainerName={}, "
                    "Status={:#x}.",
                    container_persistent_metadata.name().c_str(),
                    status);

                return status;
            }

            std::shared_ptr<container> container = container_index_->get_container(container_name);

            if (container == nullptr)
            {
                spdlog::critical("Failed to mark orphaned container as deleted on startup. "
                    "ObjectContainerName={}.",
                    container_name.c_str());

                return status;
            }

            container->mark_as_deleted();
        }
    }

    return status::success;
}

std::vector<container_partition_metadata>
container_management_service::convert_ordered_engine_references_to_container_instances(
    const std::vector<storage_engine_reference_handle*> storage_engine_references)
{
    std::vector<container_partition_metadata> container_instances;

    for (std::uint16_t collocation_index = 0u; collocation_index < storage_engine_references.size(); ++collocation_index)
    {
        container_instances.emplace_back(
            collocation_index,
            data_partition_provider_->get_partition_by_collocation(collocation_index).get_storage_engine(),
            storage_engine_references[collocation_index]);
    }

    return container_instances;
}

} // namespace storage.
} // namespace lazarus.