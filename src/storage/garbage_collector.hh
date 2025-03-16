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
    execute_garbage_collection();

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
};

} // namespace storage.
} // namespace lazarus.