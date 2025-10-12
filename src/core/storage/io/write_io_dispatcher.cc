// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'write_io_dispatcher.cc'
// Author: jcjuarez
// Description:
//      Write request dispatcher module for IO.
//      Implemented as a single-threaded lock-free
//      batching IO dispatcher.
// ****************************************************

#include <spdlog/spdlog.h>
#include "write_io_dispatcher.hh"
#include "write_batch_aggregator.hh"
#include "../../startup/lazarus_data_store.hh"

namespace lazarus::storage
{

write_io_dispatcher::write_io_dispatcher(
    std::unique_ptr<write_batch_aggregator> write_batch_aggregator)
    : write_batch_aggregator_{std::move(write_batch_aggregator)}
{}

void
write_io_dispatcher::start()
{
    spdlog::info("Starting lazarus data store write IO dispatcher thread.");

    write_dispatcher_master_thread_ = std::jthread(
        &write_io_dispatcher::dispatch_write_io_tasks,
        this,
        lazarus_data_store::get_stop_source_token());
}

void
write_io_dispatcher::enqueue_io_task(
    object_io_task&& write_io_task)
{
    //
    // This is a lock-free operation.
    //
    write_io_tasks_queue_.enqueue(std::move(write_io_task));
}

void
write_io_dispatcher::wait_for_stop()
{
    //
    // This call will make sure that the dispatcher
    // thread gets stopped, in a blocking manner.
    //
    write_dispatcher_master_thread_.join();
}

void
write_io_dispatcher::dispatch_write_io_tasks(
    std::stop_token stop_token)
{
    //
    // This is the core tight loop for dispatching write io tasks.
    //
    while (!stop_token.stop_requested())
    {
        write_batch_aggregator_->aggregate_and_commit_write_batch(
            write_io_tasks_queue_);
    }

    spdlog::info("Stopping lazarus data store write IO dispatcher thread.");
}

} // namespace lazarus::storage.