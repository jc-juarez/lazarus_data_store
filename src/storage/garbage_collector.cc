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
    garbage_collector_thread_ = std::jthread(
        &garbage_collector::execute_garbage_collection,
        this);
}

void
garbage_collector::execute_garbage_collection()
{
    spdlog::info("Garbage collector running.");
    std::this_thread::sleep_for(std::chrono::seconds(10u));
}

} // namespace storage.
} // namespace lazarus.