// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
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
#include "../../common/startable_interface.hh"
#include <moodycamel/blockingconcurrentqueue.h>

namespace lazarus::storage
{

class cache_accessor;
class data_partition_provider;
class storage_engine_interface;

class write_io_dispatcher : public io_dispatcher_interface, public common::startable_interface
{
public:

    //
    // Constructor.
    //
    write_io_dispatcher(
        data_partition_provider& data_partition_provider,
        cache_accessor& cache_accessor);

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

    void
    execute_write_io_task(
        std::unique_ptr<object_io_task> write_io_task);

    //
    // Executes an insertion operation with the storage engine.
    //
    status::status_code
    execute_insert_operation(
        storage_engine_interface& partition_storage_engine,
        storage_engine_reference* engine_reference,
        const schemas::object_request& object_request);

    //
    // Executes a removal operation with the storage engine.
    //
    status::status_code
    execute_remove_operation(
        storage_engine_interface& partition_storage_engine,
        storage_engine_reference* engine_reference,
        const schemas::object_request& object_request);

    //
    // Long-running write io dispatcher master thread.
    //
    std::jthread write_dispatcher_master_thread_;

    //
    // Lock-free and blocking queue for processing write IO operations.
    //
    moodycamel::BlockingConcurrentQueue<std::unique_ptr<object_io_task>> write_io_tasks_queue_;

    //
    // Reference for the data partition provider.
    //
    data_partition_provider& data_partition_provider_;

    //
    // Reference for the cache accessor.
    //
    cache_accessor& cache_accessor_;
};

} // namespace lazarus::storage.