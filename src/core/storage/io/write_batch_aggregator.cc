// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'write_batch_aggregator.cc'
// Author: jcjuarez
// Description:
//      Aggregates and executes batched write
//      tasks against the storage engine, and
//      synchronizes the cache if needed.
// ****************************************************

#include "object_io_executor.hh"
#include "write_batch_aggregator.hh"
#include "../cache/cache_accessor.hh"

namespace lazarus
{
namespace storage
{

write_batch_aggregator::write_batch_aggregator(
    std::shared_ptr<object_io_executor> object_io_executor,
    std::shared_ptr<cache_accessor> cache_accessor)
    : object_io_executor_{std::move(object_io_executor)},
      cache_accessor_{std::move(cache_accessor)}
{}

void
write_batch_aggregator::aggregate_and_commit_write_batch(
    moodycamel::ConcurrentQueue<object_io_task>& write_io_tasks_queue)
{}

} // namespace storage.
} // namespace lazarus.