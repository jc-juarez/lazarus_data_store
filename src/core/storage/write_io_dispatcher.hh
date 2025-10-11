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

#include "object_io_task.hh"
#include "io_dispatcher_interface.hh"
#include "../network/server/server.hh"
#include <moodycamel/concurrentqueue.h>
#include "../common/startable_interface.hh"
#include "../schemas/request-interfaces/object_request.hh"

namespace lazarus::storage
{

class write_io_dispatcher : public io_dispatcher_interface, public common::startable_interface
{
public:

    //
    // Constructor.
    //
    write_io_dispatcher(
        std::shared_ptr<storage_engine_interface> storage_engine,
        std::shared_ptr<frontline_cache> frontline_cache);

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
        object_io_task&& object_io_task) override;

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
    dispatch_write_io_operations(
        schemas::object_request&& object_request,
        std::shared_ptr<container> container,
        network::server_response_callback&& response_callback);

    //
    // Handles the insertion of elements into the frontline cache.
    //
    void
    insert_object_into_cache(
        schemas::object_request& object_request);

    //
    // Executes an insertion operation with the storage engine.
    //
    status::status_code
    execute_insert_operation(
        storage_engine_reference_handle* container_storage_engine_reference,
        const schemas::object_request& object_request);

    //
    // Executes a removal operation with the storage engine.
    //
    status::status_code
    execute_remove_operation(
        storage_engine_reference_handle* container_storage_engine_reference,
        const schemas::object_request& object_request);

    //
    // Long-running write io dispatcher master thread.
    //
    std::jthread write_dispatcher_master_thread_;

    //
    // Lock-free and non-blocking queue for processing write IO operations.
    //
    moodycamel::ConcurrentQueue<object_io_task> write_io_tasks_queue_;

    //
    // Reference for the storage engine component.
    //
    std::shared_ptr<storage_engine_interface> storage_engine_;

    //
    // Frontline cache handle.
    //
    std::shared_ptr<storage::frontline_cache> frontline_cache_;
};

} // namespace lazarus::common.