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

namespace lazarus::storage
{

class storage_engine;
class object_container;
class object_container_index;

class orphaned_container_scavenger
{
public:

    //
    // Constructor.
    //
    orphaned_container_scavenger(
        std::shared_ptr<storage_engine> storage_engine,
        std::shared_ptr<object_container_index> object_container_index);

    //
    // Scans the given object containers and cleans them up if necessary.
    //
    void
    cleanup_orphaned_containers(
        const std::uint16_t container_bucket_index,
        const std::uint64_t garbage_collector_iteration_count,
        const std::vector<std::shared_ptr<object_container>>& object_containers);

private:

    //
    // Handle for the storage engine.
    //
    std::shared_ptr<storage_engine> storage_engine_;

    //
    // Handle for the object container index.
    //
    std::shared_ptr<object_container_index> object_container_index_;
};

} // namespace lazarus::storage.