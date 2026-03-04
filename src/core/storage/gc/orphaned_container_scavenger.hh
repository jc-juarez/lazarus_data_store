// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'orphaned_container_scavenger.hh'
// Author: jcjuarez
// Description:
//      Scavenger to clean up rogue containers from
//      both the storage engine and metadata table.
// ****************************************************

#pragma once

#include <vector>
#include <memory>
#include "../models/container_partition_metadata.hh"

namespace lazarus::storage
{

class container;
class container_index;

class orphaned_container_scavenger
{
public:

    //
    // Constructor.
    //
    orphaned_container_scavenger(
        std::shared_ptr<container_index> container_index);

    //
    // Scans the given object containers and cleans them up if necessary.
    //
    void
    cleanup_orphaned_containers(
        const std::uint16_t container_bucket_index,
        const std::uint64_t garbage_collector_iteration_count,
        const std::vector<std::shared_ptr<container>>& containers);

private:

    //
    // Deletes the container instances.
    //
    status::status_code
    delete_container_instances_from_data_partitions(
        std::vector<container_partition_metadata> container_instances);

    //
    // Handle for the object container index.
    //
    std::shared_ptr<container_index> container_index_;
};

} // namespace lazarus::storage.