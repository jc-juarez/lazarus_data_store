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
    std::shared_ptr<storage_engine> storage_engine_handle,
    std::shared_ptr<object_container_index> object_container_index_handle)
    : storage_engine_{std::move(storage_engine_handle)},
      object_container_index_{std::move(object_container_index_handle)}
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
        spdlog::info("Garbage collector running.");
        std::this_thread::sleep_for(std::chrono::seconds(10u));
    }

    spdlog::info("Stopping lazarus data store garbage collector thread.");
}

} // namespace storage.
} // namespace lazarus.