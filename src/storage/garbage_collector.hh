// ****************************************************
// Lazarus Data Store
// Storage
// 'garbage_collector.hh'
// Author: jcjuarez
// Description:
//      Garbage collector for cleaning up stale
//      resources.
// ****************************************************

#pragma once

#include <thread>
#include <memory>
#include <stop_token>
#include "storage_configuration.hh"
#include "../common/alertable_sleeper.hh"

namespace lazarus
{
namespace storage
{

class storage_engine;
class container_index;
class orphaned_container_scavenger;

//
// Garbage collector in charge of cleaning up
// stale resources in the system.
//
class garbage_collector
{
public:

    //
    // Constructor for the garbage collector.
    //
    garbage_collector(
        const storage_configuration& storage_configuration,
        std::shared_ptr<container_index> container_index,
        std::unique_ptr<orphaned_container_scavenger> orphaned_container_scavenger);

    //
    // Starts the long-running garbage collector thread.
    //
    void
    start();

private:

    //
    // Long-running thread entry point.
    //
    void
    execute_garbage_collection(
        std::stop_token stop_token);

    //
    // Cleans up any orphaned object containers.
    // Deletion from both the filesystem and index metadata table occurs.
    //
    void
    cleanup_orphaned_object_containers();

    //
    // Handle for the object container index.
    //
    std::shared_ptr<container_index> object_container_index_;

    //
    // Handle for the container scavenger component.
    //
    std::unique_ptr<orphaned_container_scavenger> orphaned_container_scavenger_;

    //
    // Long-running garbage collector thread handle.
    //
    std::jthread garbage_collector_thread_;

    //
    // Configuration for the storage subsystem.
    //
    const storage_configuration storage_configuration_;

    //
    // In-memory only iteration counter for the garbage collector thread.
    // Resets on every startup.
    //
    std::uint64_t iteration_count_;

    //
    // Alertable sleeper for stopping midway sleep cycles.
    //
    common::alertable_sleeper alertable_sleeper_;
};

} // namespace storage.
} // namespace lazarus.