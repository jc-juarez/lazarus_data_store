// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'orphaned_container_scavenger.cc'
// Author: jcjuarez
// Description:
//      Scavenger to clean up rogue containers from
//      both the storage engine and metadata table.
// ****************************************************

#include <spdlog/spdlog.h>
#include "../models/container.hh"
#include "../io/storage_engine.hh"
#include "../index/container_index.hh"
#include "orphaned_container_scavenger.hh"

namespace lazarus::storage
{

orphaned_container_scavenger::orphaned_container_scavenger(
    std::shared_ptr<data_partition_provider> data_partition_provider,
    std::shared_ptr<container_index> container_index)
    : data_partition_provider_{std::move(data_partition_provider)},
      container_index_{std::move(container_index)}
{}

void
orphaned_container_scavenger::cleanup_orphaned_containers(
    const std::uint16_t container_bucket_index,
    const std::uint64_t garbage_collector_iteration_count,
    const std::vector<std::shared_ptr<container>>& containers)
{
    if (containers.empty())
    {
        //
        // No object containers present.
        // Nothing to do here.
        //
        spdlog::info("No object containers found during garbage collection. "
            "ContainerBucketIndexBeingTraversed={}, "
            "GarbageCollectorCurrentIteration={}.",
            container_bucket_index,
            garbage_collector_iteration_count);

        return;
    }

    spdlog::info("Found object containers during garbage collection. "
        "NumberObjectContainers={}, "
        "ContainerBucketIndexBeingTraversed={}, "
        "GarbageCollectorCurrentIteration={}.",
        containers.size(),
        container_bucket_index,
        garbage_collector_iteration_count);

    //
    // Keep track of all deleted object containers
    // in the current garbage collection iteration.
    //
    std::uint32_t number_cleaned_up_containers = 0;

    for (auto &container: containers)
    {
        //
        // If the object container is marked as deleted,
        // it needs to be deleted from the filesystem and index table.
        //
        if (container->is_deleted())
        {
            spdlog::info("Found orphaned object container during garbage collection. "
                "Attempting to delete it. "
                "ObjectContainerMetadata={}, "
                "ContainerBucketIndexBeingTraversed={}, "
                "GarbageCollectorCurrentIteration={}.",
                container->to_string(),
                container_bucket_index,
                garbage_collector_iteration_count);

            status::status_code status = delete_container_instances_from_data_partitions(
                container->get_container_instances());

            if (status::failed(status))
            {
                //
                // Object container deletion failed in the storage engine; skip entry.
                //
                spdlog::error("Failed to remove object container from the data partitions. "
                    "ObjectContainerMetadata={}, "
                    "ContainerBucketIndexBeingTraversed={}, "
                    "GarbageCollectorCurrentIteration={}, "
                    "Status={:#x}.",
                    container->to_string(),
                    container_bucket_index,
                    garbage_collector_iteration_count,
                    status);

                continue;
            }

            //
            // At this point, the object container has been deleted
            // from the filesystem. Safe to delete the in-memory reference now.
            //
            status = container_index_->remove_container(
                container->get_name().c_str());

            if (status::failed(status))
            {
                spdlog::error("Failed to remove object container from the index metadata table. "
                    "ObjectContainerMetadata={}, "
                    "ContainerBucketIndexBeingTraversed={}, "
                    "GarbageCollectorCurrentIteration={}, "
                    "Status={:#x}.",
                    container->to_string(),
                    container_bucket_index,
                    garbage_collector_iteration_count,
                    status);
            }
            else
            {
                ++number_cleaned_up_containers;

                spdlog::info("Object container has been successfully deleted from the storage "
                    "engine and the index metadata table. Memory will be freed after all references are dropped. "
                    "ObjectContainerMetadata={}, "
                    "ContainerBucketIndexBeingTraversed={}, "
                    "GarbageCollectorCurrentIteration={}.",
                    container->to_string(),
                    container_bucket_index,
                    garbage_collector_iteration_count);
            }
        }
    }

    if (number_cleaned_up_containers == 0)
    {
        spdlog::info("No orphaned object containers cleaned up during garbage collection. "
            "ContainerBucketIndexBeingTraversed={}, "
            "GarbageCollectorCurrentIteration={}.",
            container_bucket_index,
            garbage_collector_iteration_count);
    }
    else
    {
        spdlog::info("Successfully cleaned up orphaned object containers found during garbage collection. "
            "NumberObjectContainersCleanedUp={}, "
            "ContainerBucketIndexBeingTraversed={}, "
            "GarbageCollectorCurrentIteration={}.",
            number_cleaned_up_containers,
            container_bucket_index,
            garbage_collector_iteration_count);
    }
}

status::status_code
orphaned_container_scavenger::delete_container_instances_from_data_partitions(
    std::vector<container_partition_metadata> container_instances)
{
    for (auto& container_instance : container_instances)
    {
        status::status_code status = container_instance.storage_engine_.remove_container(
            container_instance.storage_engine_reference_);

        if (status::failed(status))
        {
            spdlog::error("Failed to remove object container from the data partition. "
                "DataPartitionCollocationIndex={}, "
                "StorageEngineReference={}.",
                container_instance.collocation_index_,
                static_cast<void*>(container_instance.storage_engine_reference_));

            return status;
        }
    }

    return status::success;
}

} // namespace lazarus::storage.