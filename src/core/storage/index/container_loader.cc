// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_loader.cc'
// Author: jcjuarez
// Description:
//      Orchestrates the logic for loading containers
//      found on the filesystem into the index.
// ****************************************************

#include "container_index.hh"
#include "container_loader.hh"
#include "container_registry.hh"
#include "../io/data_partition.hh"
#include "../io/data_partition_provider.hh"

namespace lazarus
{
namespace storage
{

container_loader::container_loader(
    data_partition& metadata_partition,
    container_index& container_index,
    data_partition_provider& data_partition_provider)
    : metadata_partition_{metadata_partition},
      container_index_{container_index},
      data_partition_provider_{data_partition_provider}
{}

status::status_code
container_loader::load_container_index(
    std::unordered_map<std::string, storage_engine_reference*>& metadata_partition_references,
    container_registry& structured_partitions_registry)
{
    if (metadata_partition_references.size() == 0u ||
        metadata_partition_references.size() > k_max_metadata_partition_containers)
    {
        //
        // The container metadata partition should either have the default
        // container, or the default container plus the container metadata container.
        //
        TRACE_LOG(critical, "Unexpected number of containers found for the metadata partition. "
            "MaxNumContainers={}, "
            "NumContainersFound={}.",
            k_max_metadata_partition_containers,
            metadata_partition_references.size());

        return status::unexpected_metadata_partition_number_containers;
    }

    //
    // Keep track of the internal metadata object containers.
    // The only case when it is valid for them to be absent is when the column
    // family references mapping only has 1 entry (kDefaultColumnFamilyName), which
    // implies that the data store is starting up for the first time.
    //
    if (metadata_partition_references.size() == 1u)
    {
        //
        // This means this is the first ever startup for the data store,
        // or that it simply does not need a previous metadata state for working.
        // Create all required internal metadata root object containers.
        //
        create_container_metadata_column_family(&metadata_partition_references);
    }

    //
    // Ensure the container metadata container is present; if not, this is a
    // critical error as the data store cannot function without this critical metadata.
    //
    if (metadata_partition_references.find(k_containers_container_name_metadata_partition) ==
        metadata_partition_references.end())
    {
        //
        // This is a critical error as the persistent container metadata to discover
        // previously existing containers is not present. Fail the system startup.
        //
        TRACE_LOG(critical, "Failed to find find the storage engine reference for the '{}' internal metadata.",
            k_containers_container_name_metadata_partition);

        return status::containers_internal_metadata_lookup_failed;
    }

    storage_engine_reference* container_metadata_engine_reference =
        metadata_partition_references.at(k_containers_container_name_metadata_partition);

    //
    // Get all known object containers to the system from the
    // persistent container metadata and populate the rest of the object container index.
    //
    std::unordered_map<std::string, byte_stream> containers_present_on_metadata;
    status::status_code status = metadata_partition_.get_storage_engine().get_all_objects_from_container(
        container_metadata_engine_reference,
        &containers_present_on_metadata);

    if (status::failed(status))
    {
        TRACE_LOG(critical, "Failed to get all container names from the container metadata partition. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // Index all container metadata containers.
    //
    status = index_containers_from_metadata_partition(
        metadata_partition_references);

    if (status::failed(status))
    {
        TRACE_LOG(critical, "Failed to index the container metadata internal containers. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // Index all containers known to the persistent
    // container metadata into the container index.
    //
    status = index_containers_from_structured_data_partitions(
        structured_partitions_registry,
        containers_present_on_metadata);

    if (status::failed(status))
    {
        TRACE_LOG(critical, "Failed to index the containers known to the persistent container metadata. "
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
        TRACE_LOG(critical, "Failed to scan and index orphaned containers during startup. "
            "Status={:#x}.",
            status);

        return status;
    }

    //
    // As a final sanity check, ensure that every engine reference
    // for every container matches the expected data partition.
    //
    status = validate_loaded_engine_references();

    if (status::failed(status))
    {
        TRACE_LOG(critical, "Failed to validate loaded engine references during startup. "
            "Status={:#x}.",
            status);

        return status;
    }

    return status::success;
}

status::status_code
container_loader::create_container_metadata_column_family(
    std::unordered_map<std::string, storage_engine_reference*>* metadata_partition_references)
{
    //
    // Create the object containers column family on the storage engine.
    //
    storage_engine_reference* container_metadata_engine_reference;
    status::status_code status = metadata_partition_.get_storage_engine().create_container(
        k_containers_container_name_metadata_partition,
        &container_metadata_engine_reference);

    if (status::failed(status))
    {
        TRACE_LOG(critical, "Failed to create internal metadata column family '{}'.",
            k_containers_container_name_metadata_partition);

        return status;
    }

    //
    // This authoritative metadata container must be considered as an approved reference.
    //
    metadata_partition_.get_storage_engine().register_approved_engine_references(
        {container_metadata_engine_reference});

    //
    // All structured data partitions which thus container tracks will always spin up
    // the default column family, so it should be registered inside this container.
    //
    const schemas::container_persistent_interface container_persistent_metadata =
        container::create_container_persistent_metadata(rocksdb::kDefaultColumnFamilyName.c_str());
    byte_stream serialized_container_persistent_metadata;
    container_persistent_metadata.SerializeToString(&serialized_container_persistent_metadata);
    status = metadata_partition_.get_storage_engine().insert_object(
        container_metadata_engine_reference,
        rocksdb::kDefaultColumnFamilyName.c_str(),
        serialized_container_persistent_metadata);

    if (status::failed(status))
    {
        TRACE_LOG(critical, "Failed to insert default column family for the container metadata container.");

        return status;
    }

    //
    // Append to the storage references mapping for the container metadata partition.
    //
    metadata_partition_references->emplace(
        k_containers_container_name_metadata_partition,
        container_metadata_engine_reference);

    return status::success;
}

status::status_code
container_loader::index_containers_from_metadata_partition(
    std::unordered_map<std::string, storage_engine_reference*>& metadata_partition_references)
{
    for (auto& container_to_index : metadata_partition_references)
    {
        //
        // Containers on the metadata partition only live under such partition,
        // so it is only needed to pass down the respective storage engine reference for such partition.
        //
        container_instance container_metadata {
            metadata_partition_.get_collocation_index(),
            metadata_partition_.get_storage_engine(),
            container_to_index.second};
            std::vector<container_instance> container_instances {container_metadata};

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
        status::status_code status = container_index_.insert_container(
            container_persistent_metadata,
            container_instances);

        if (status::failed(status))
        {
            TRACE_LOG(critical, "Failed to insert container from container metadata into the container index. "
                "ContainerName={}, "
                "Status={:#x}.",
                container_persistent_metadata.name().c_str(),
                status);

            return status;
        }

        const std::shared_ptr<container> container =
            container_index_.get_container(container_name);

        TRACE_LOG(info, "Indexed internal metadata container on startup. "
            "ContainerMetadata={}.",
            container->to_string());
    }

    return status::success;
}

status::status_code
container_loader::index_containers_from_structured_data_partitions(
    container_registry& structured_partitions_registry,
    std::unordered_map<std::string, byte_stream>& containers_present_on_metadata)
{
    for (auto& container_present_on_metadata : containers_present_on_metadata)
    {
        const std::string& container_name = container_present_on_metadata.first;
        const byte_stream& container_raw_metadata = container_present_on_metadata.second;
        const std::optional<std::vector<storage_engine_reference*>> engine_references =
            structured_partitions_registry.get_references(container_name);

        if (engine_references == std::nullopt)
        {
            //
            // This is a critical inconsistency problem.
            // This implies that the persistent container metadata has a record of a container
            // which is not present on the filesystem across the structured data partitions.
            //
            TRACE_LOG(critical, "Failed to locate a container known to the persistent container metadata "
                "on the filesystem across the structured data partitions. "
                "ContainerName={}.",
                container_name);

            return status::missing_container_on_data_partitions;
        }

        //
        // Perform an integrity validation for this container found inside the structured
        // data partitions. Startup should be stopped on inconsistencies or corruption.
        // Given this container is present on the metadata after a WDT (Well Defined Transaction),
        // the data integrity for this container is crucial for system startup.
        // This integrity validation is only needed for those containers known to the persistent metadata.
        // All other containers not present on the metadata are considered as dirty filesystem state,
        // so their integrity is irrelevant for system startup.
        //
        status::status_code status = structured_partitions_registry.execute_integrity_validation(
            container_name);

        if (status::failed(status))
        {
            TRACE_LOG(critical, "Integrity validation for the container name on the "
                "structured data partitions containers failed. "
                "ContainerName={}, "
                "Status={:#x}.",
                container_name,
                status);

            return status;
        }

        schemas::container_persistent_interface container_persistent_metadata;
        const bool is_parsing_successful = container_persistent_metadata.ParseFromString(container_raw_metadata);
        if (!is_parsing_successful)
        {
            TRACE_LOG(critical, "Failed to parse a container raw metadata on startup. "
                "ContainerName={}.",
                container_name);

            return status::parsing_failed;
        }

        std::vector<container_instance> container_instances =
            convert_ordered_engine_references_to_container_instances(engine_references.value());

        status = container_index_.insert_container(
            container_persistent_metadata,
            container_instances);

        if (status::failed(status))
        {
            TRACE_LOG(critical, "Failed to insert container into the container index. "
                "ContainerName={}, "
                "Status={:#x}.",
                container_persistent_metadata.name().c_str(),
                status);

            return status;
        }

        const std::shared_ptr<container> container =
            container_index_.get_container(container_name);

        TRACE_LOG(info, "Found container on structured data partitions during startup and indexed into "
            "the object containers metadata table. "
            "ContainerMetadata={}.",
            container->to_string());
    }

    return status::success;
}

status::status_code
container_loader::scan_and_index_orphaned_containers(
    container_registry& structured_partitions_registry,
    std::unordered_map<std::string, byte_stream>& containers_present_on_metadata)
{
    for (const auto& registry_entry : structured_partitions_registry)
    {
        const std::string& container_name = registry_entry.first;
        const std::vector<storage_engine_reference*>& engine_references = registry_entry.second;

        if (containers_present_on_metadata.find(container_name) == containers_present_on_metadata.end())
        {
            //
            // This implies this is an orphaned container present on the structured data partitions on the
            // filesystem, but the persistent container metadata is not aware of it. Mark it as deleted for
            // the garbage collector to clean it up later.
            //
            TRACE_LOG(warn, "Found orphaned object container on startup to be cleaned up by the garbage collector. "
                "ObjectContainerName={}.",
                container_name.c_str());

            std::vector<container_instance> container_instances =
                convert_ordered_engine_references_to_container_instances(engine_references);

            const schemas::container_persistent_interface container_persistent_metadata =
                container::create_container_persistent_metadata(container_name.c_str());
            status::status_code status = container_index_.insert_container(
                container_persistent_metadata,
                container_instances);

            if (status::failed(status))
            {
                TRACE_LOG(critical, "Failed to insert orphaned container into the container index. "
                    "ContainerName={}, "
                    "Status={:#x}.",
                    container_persistent_metadata.name().c_str(),
                    status);

                return status;
            }

            std::shared_ptr<container> container = container_index_.get_container(container_name);

            if (container == nullptr)
            {
                TRACE_LOG(critical, "Failed to mark orphaned container as deleted on startup. "
                    "ObjectContainerName={}.",
                    container_name.c_str());

                return status;
            }

            container->mark_as_deleted();
        }
    }

    return status::success;
}

std::vector<container_instance>
container_loader::convert_ordered_engine_references_to_container_instances(
const std::vector<storage_engine_reference*> storage_engine_references)
{
    std::vector<container_instance> container_instances;

    for (std::uint16_t collocation_index = 0u; collocation_index < storage_engine_references.size(); ++collocation_index)
    {
        container_instances.emplace_back(
            collocation_index,
            data_partition_provider_.get_partition_by_collocation(collocation_index).get_storage_engine(),
            storage_engine_references[collocation_index]);
    }

    return container_instances;
}

status::status_code
container_loader::validate_loaded_engine_references()
{
    //
    // Traverse the complete index.
    // For every container instance, ensure the engine reference is
    // correctly associated to the expected data partition and confirm
    // there are no other data partitions considering such reference as approved.
    //
    for (std::uint16_t bucket_index = 0; bucket_index < container_index_.get_number_container_buckets(); ++bucket_index)
    {
        std::vector<std::shared_ptr<container>> containers =
            container_index_.get_all_containers_from_bucket(bucket_index);

        for (auto& container : containers)
        {
            status::status_code status = validate_container_engine_references(
                container);

            if (status::failed(status))
            {
                TRACE_LOG(critical, "Failed to validate engine references for container during startup. "
                    "ContainerMetadata={}.",
                    container->to_string());

                return status;
            }
        }
    }

    return status::success;
}

status::status_code
container_loader::validate_container_engine_references(
    std::shared_ptr<container> container)
{
    const bool is_orphaned_container = container->is_deleted();
    std::vector<container_instance> container_instances = container->get_container_instances();

    for (auto& container_instance : container_instances)
    {
        if (container_instance.storage_engine_reference_ == nullptr)
        {
            //
            // If the engine reference for the given instance is marked as null,
            // this must correspond to an orphaned container. This is valid and should be
            // allowed, but only for orphaned containers. If this is not an orphaned container,
            // the startup process should be halted.
            //
            if (!is_orphaned_container)
            {
                TRACE_LOG(critical, "Found null engine reference for a non-orphaned container during startup. "
                    "ContainerMetadata={}, "
                    "CollocationIndex={}.",
                    container->to_string(),
                    container_instance.collocation_index_);

                return status::null_engine_reference_non_orphaned_container;
            }
        }
        else
        {
            //
            // For all other references that are not null, validate:
            // 1. Reference is approved for the expected storage engine.
            // 2. Reference is not approved for any of the other storage engines.
            //
            const std::span<data_partition> data_partitions =
                data_partition_provider_.get_all_partitions();

            for (auto& partition : data_partitions)
            {
                storage_engine_interface& engine = partition.get_storage_engine();

                if (partition.get_collocation_index() == container_instance.collocation_index_)
                {
                    //
                    // This is the expected data partition.
                    // Should be present here.
                    //
                    if (!engine.fence_engine_reference(container_instance.storage_engine_reference_))
                    {
                        TRACE_LOG(critical, "Engine reference is not registered with the expected storage engine. "
                            "EngineReference={}, "
                            "ContainerMetadata={}, "
                            "CollocationIndex={}.",
                            static_cast<void*>(container_instance.storage_engine_reference_),
                            container->to_string(),
                            container_instance.collocation_index_);

                        return status::storage_engine_reference_not_approved;
                    }

                    //
                    // Also make sure that the engine baked into the instance structure
                    // maps to the expected engine instance.
                    //
                    if (&container_instance.storage_engine_ != &engine)
                    {
                        return status::unexpected_storage_engine;
                    }
                }
                else
                {
                    //
                    // These are the rest of partitions.
                    // Should not be present here.
                    //
                    if (engine.fence_engine_reference(container_instance.storage_engine_reference_))
                    {
                        TRACE_LOG(critical, "Engine reference is registered with an unexpected storage engine. "
                            "EngineReference={}, "
                            "ContainerMetadata={}, "
                            "CollocationIndex={}.",
                            static_cast<void*>(container_instance.storage_engine_reference_),
                            container->to_string(),
                            container_instance.collocation_index_);

                        return status::storage_engine_reference_unexpectedly_approved;
                    }
                }
            }
        }
    }

    return status::success;
}

} // namespace storage.
} // namespace lazarus.