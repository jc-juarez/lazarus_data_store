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
#include "object_container_index.hh"
#include "../main/lazarus_data_store.hh"

namespace lazarus
{
namespace storage
{

garbage_collector::garbage_collector(
    const storage_configuration& storage_configuration,
    std::shared_ptr<storage_engine> storage_engine_handle,
    std::shared_ptr<object_container_index> object_container_index_handle)
    : storage_configuration_{storage_configuration},
      storage_engine_{std::move(storage_engine_handle)},
      object_container_index_{std::move(object_container_index_handle)},
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
            "CurrentIteration={}.",
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
            "CurrentIteration={}.",
            iteration_count_);

        ++iteration_count_;

        std::this_thread::sleep_for(std::chrono::milliseconds(
            storage_configuration_.garbage_collector_periodic_interval_ms_));
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
    std::vector<std::shared_ptr<object_container>> object_containers =
        object_container_index_->get_all_object_containers();

    if (object_containers.empty())
    {
        //
        // No object containers present.
        // Nothing to do here.
        //
        spdlog::info("No object containers found during garbage collection. "
            "CurrentIteration={}.",
            iteration_count_);

        return;
    }

    spdlog::info("Found object containers during garbage collection. "
        "NumberObjectContainers={}, "
        "CurrentIteration={}.",
        object_containers.size(),
        iteration_count_);

    //
    // Keep track of all deleted object containers
    // in the current garbage collection iteration.
    //
    std::uint32_t number_cleaned_up_object_containers = 0;

    for (auto& object_container : object_containers)
    {
        //
        // If the object container is marked as deleted,
        // it needs to be deleted from the filesystem and index table.
        //
        if (object_container->is_deleted())
        {
            spdlog::info("Found orphaned object container during garbage collection. "
                "Attempting to delete it. "
                "ObjectContainerMetadata={}.",
                object_container->to_string());

            status::status_code status = storage_engine_->remove_object_container(
                object_container->get_storage_engine_reference());

            if (status::failed(status))
            {
                //
                // Object container deletion failed in the storage engine; skip entry.
                //
                spdlog::error("Failed to remove object container from the storage engine. "
                    "ObjectContainerMetadata={}, "
                    "Status={:#x}.",
                    object_container->to_string(),
                    status);

                continue;
            }

            //
            // At this point, the object container has been deleted
            // from the filesystem. Safe to delete the in-memory reference now.
            //
            status = object_container_index_->remove_object_container(
                object_container->get_name().c_str());

            if (status::failed(status))
            {
                spdlog::error("Failed to remove object container from the index metadata table. "
                    "ObjectContainerMetadata={}, "
                    "Status={:#x}.",
                    object_container->to_string(),
                    status);
            }
            else
            {
                ++number_cleaned_up_object_containers;

                spdlog::info("Object container has been successfully deleted from the storage "
                    "engine and the index metadata table. Memory will be freed after all references are dropped. "
                    "ObjectContainerMetadata={}.",
                    object_container->to_string());
            }
        }
    }

    if (number_cleaned_up_object_containers == 0)
    {
        spdlog::info("No orphaned object containers cleaned up during garbage collection. "
            "CurrentIteration={}.",
            iteration_count_);
    }
    else
    {
        spdlog::info("Successfully cleaned up orphaned object containers found during garbage collection. "
            "NumberObjectContainersCleanedUp={}, "
            "CurrentIteration={}.",
            number_cleaned_up_object_containers,
            iteration_count_);
    }
}

} // namespace storage.
} // namespace lazarus.