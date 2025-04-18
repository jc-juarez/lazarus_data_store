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

namespace lazarus
{
namespace storage
{

class storage_engine;
class object_container_index;

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
        std::shared_ptr<storage_engine> storage_engine_handle,
        std::shared_ptr<object_container_index> object_container_index_handle);

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
    // Handle for the storage enine.
    //
    std::shared_ptr<storage_engine> storage_engine_;

    //
    // Handle for the object container index.
    //
    std::shared_ptr<object_container_index> object_container_index_;

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
};

} // namespace storage.
} // namespace lazarus.