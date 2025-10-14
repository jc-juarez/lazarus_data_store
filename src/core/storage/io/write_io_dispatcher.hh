// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'write_io_dispatcher.hh'
// Author: jcjuarez
// Description:
//      Write request dispatcher module for IO.
//      Implemented as a single-threaded lock-free
//      batching IO dispatcher.
// ****************************************************

#pragma once

#include "io_dispatcher_interface.hh"
#include "../models/object_io_task.hh"
#include <moodycamel/concurrentqueue.h>
#include "../../common/startable_interface.hh"

namespace lazarus::storage
{

class write_batch_aggregator;

class write_io_dispatcher : public io_dispatcher_interface, public common::startable_interface
{
public:

    //
    // Constructor.
    //
    write_io_dispatcher(
        std::unique_ptr<write_batch_aggregator> write_batch_aggregator);

    //
    // Starts the write dispatcher master thread.
    //
    void
    start() override;

    //
    // Enqueues a write IO operation for to be processed
    // by the dispatcher in a single-threaded batching model.
    //
    void
    enqueue_io_task(
        object_io_task&& write_io_task) override;

    //
    // Waits for the write dispatcher master
    // thread to finish execution in a blocking manner.
    //
    void
    wait_for_stop() override;

private:

    //
    // Write IO requests dispatcher entry point.
    // This is the long-running dispatcher master thread context.
    //
    void
    dispatch_write_io_tasks(
        std::stop_token stop_token);

    //
    // Long-running write io dispatcher master thread.
    //
    std::jthread write_dispatcher_master_thread_;

    //
    // Lock-free and non-blocking queue for processing write IO operations.
    //
    moodycamel::ConcurrentQueue<object_io_task> write_io_tasks_queue_;

    //
    // Write batch aggregator handle.
    //
    std::unique_ptr<write_batch_aggregator> write_batch_aggregator_;
};

} // namespace lazarus::storage.