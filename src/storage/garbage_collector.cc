// ****************************************************
// Lazarus Data Store
// Storage
// 'garbage_collector.cc'
// Author: jcjuarez
// Description:
//      Garbage collector for cleaning up stale
//      resources.
// ****************************************************

#include <chrono>
#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "garbage_collector.hh"
#include "container_index.hh"
#include "../main/lazarus_data_store.hh"
#include "orphaned_container_scavenger.hh"

namespace lazarus
{
namespace storage
{

garbage_collector::garbage_collector(
    const storage_configuration& storage_configuration,
    std::shared_ptr<container_index> object_container_index,
    std::unique_ptr<orphaned_container_scavenger> orphaned_container_scavenger)
    : storage_configuration_{storage_configuration},
      object_container_index_{std::move(object_container_index)},
      orphaned_container_scavenger_{std::move(orphaned_container_scavenger)},
      iteration_count_{0u}
{}

void
garbage_collector::start()
{
    spdlog::info("Starting lazarus data store garbage collector thread.");

    garbage_collector_thread_ = std::jthread(
        &garbage_collector::execute_garbage_collection,
        this,
        lazarus_data_store::get_stop_source_token());
}

void
garbage_collector::execute_garbage_collection(
    std::stop_token stop_token)
{
    while (!stop_token.stop_requested())
    {
        spdlog::info("Starting garbage collection iteration. "
            "GarbageCollectorCurrentIteration={}.",
            iteration_count_);

        //
        // Remove any orphaned object containers from the filesystem
        // and index metadata table.
        //
        cleanup_orphaned_object_containers();

        //
        // Iteration finished. increment counter and put thread into idle mode.
        //
        spdlog::info("Finishing garbage collection iteration. "
            "GarbageCollectorCurrentIteration={}.",
            iteration_count_);

        ++iteration_count_;

        const bool is_stop_requested = alertable_sleeper_.wait_for_and_alert_if_stopped(
            stop_token,
            storage_configuration_.garbage_collector_periodic_interval_ms_);

        if (is_stop_requested)
        {
            //
            // Stop has been requested; stop the garbage collector.
            //
            break;
        }
    }

    spdlog::info("Stopping lazarus data store garbage collector thread.");
}

void
garbage_collector::cleanup_orphaned_object_containers()
{
    //
    // By definition, an orphaned object container is any object
    // container present in the filesystem according to the storage engine
    // which is not present in the object containers internal metadata.
    // In essence, this means there is a broken connection between storage engine
    // and metadata, so any object containers not known to the metadata need to be
    // cleaned up from the filesystem along their in-memory references from the index table.
    // Orphaned object containers are only discovered during startup and will be marked as deleted,
    // or in case the object container was deleted at runtime, it will also be marked as deleted.
    //
    for (std::uint16_t bucket_index = 0; bucket_index < object_container_index_->get_number_container_buckets(); ++bucket_index)
    {
        std::vector<std::shared_ptr<container>> object_containers =
            object_container_index_->get_all_object_containers_from_bucket(bucket_index);

        spdlog::info("Scanning container bucket to look for orphaned containers. "
            "ContainerBucketIndexBeingTraversed={}, "
            "GarbageCollectorCurrentIteration={}.",
            bucket_index,
            iteration_count_);

        orphaned_container_scavenger_->cleanup_orphaned_containers(
            bucket_index,
            iteration_count_,
            object_containers);
    }
}

} // namespace storage.
} // namespace lazarus.