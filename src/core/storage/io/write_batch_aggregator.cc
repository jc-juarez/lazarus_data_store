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

#include "read_io_executor.hh"
#include "write_batch_aggregator.hh"
#include "../cache/cache_accessor.hh"

namespace lazarus
{
namespace storage
{

write_batch_aggregator::write_batch_aggregator(
    read_io_executor& object_io_executor,
    cache_accessor& cache_accessor)
    : object_io_executor_{object_io_executor},
      cache_accessor_{cache_accessor}
{}

/*
void
write_batch_aggregator::aggregate_and_commit_write_batch(
    moodycamel::ConcurrentQueue<object_io_task>& write_io_tasks_queue)
{
    std::array<object_io_task, 50u> buffer;
    size_t count = 0;

    // Try to dequeue up to kMaxBatchSize tasks.
    while (count < 50u && write_io_tasks_queue.try_dequeue(buffer[count]))
    {
        ++count;
    }

    // Nothing to write — return early.
    if (count == 0)
        return;

    // Construct a RocksDB batch (stack allocated).
    rocksdb::WriteBatch write_batch{};
    write_batch.Data().size();
    //write_batch.(count); // optional, preallocs internal structures

    // Aggregate tasks into the batch.
    for (size_t i = 0; i < count; ++i)
    {
        const auto& task = buffer[i];

        // Example: add the CF/key/value into the batch
        write_batch.Put(
        rocksdb::ColumnFamilyHandle*
        task.key,
        task.value);

        // (Optional) If task is a deletion:
        // write_batch.Delete(cf_handle, task.key);
    }

    // Commit batch to the KV store.
    execute_objects_write_batch(write_batch);

    // Optionally: reply to callbacks (if task carries a drogon::response)
    for (size_t i = 0; i < count; ++i)
    {
        const auto& task = buffer[i];
        if (task.response_callback)
        {
            task.response_callback();
        }
    }
}
*/

} // namespace storage.
} // namespace lazarus.