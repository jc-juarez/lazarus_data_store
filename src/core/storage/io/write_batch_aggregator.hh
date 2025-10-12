// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'write_batch_aggregator.hh'
// Author: jcjuarez
// Description:
//      Aggregates and executes batched write
//      tasks against the storage engine, and
//      synchronizes the cache if needed.
// ****************************************************

#pragma once

#include "../../status/status.hh"
#include "../../common/aliases.hh"
#include "../models/object_io_task.hh"
#include <moodycamel/concurrentqueue.h>

namespace lazarus
{
namespace storage
{

class cache_accessor;
class object_io_executor;

class write_batch_aggregator
{
public:

    //
    // Constructor.
    //
    write_batch_aggregator(
        std::shared_ptr<object_io_executor> object_io_executor,
        std::shared_ptr<cache_accessor> cache_accessor);

    //
    // Aggregates write IO tasks from a given queue and
    // commits a write batch operation against the storage engine.
    // Also executes a cache synchronization if needed.
    //
    void
    aggregate_and_commit_write_batch(
        moodycamel::ConcurrentQueue<object_io_task>& write_io_tasks_queue);

private:

    //
    // Object IO executor handle.
    //
    std::shared_ptr<object_io_executor> object_io_executor_;

    //
    // Cache accessor handle.
    //
    std::shared_ptr<cache_accessor> cache_accessor_;
};

} // namespace storage.
} // namespace lazarus.