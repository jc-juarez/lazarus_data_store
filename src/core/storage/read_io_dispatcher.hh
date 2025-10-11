// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'read_io_dispatcher.hh'
// Author: jcjuarez
// Description:
//      Read request dispatcher module for IO.
//      Implemented as a thread pool dispatcher.
// ****************************************************

#pragma once

#include <boost/asio.hpp>
#include "io_dispatcher_interface.hh"
#include "../network/server/server.hh"
#include "../schemas/request-interfaces/object_request.hh"

namespace lazarus::storage
{

class read_io_dispatcher : public io_dispatcher_interface
{
public:

    //
    // Constructor.
    //
    read_io_dispatcher(
        const std::uint32_t number_read_io_threads,
        std::shared_ptr<storage_engine_interface> storage_engine,
        std::shared_ptr<frontline_cache> frontline_cache);

    //
    // Enqueues a read IO operation for to be processed
    // by the dispatcher within a thread pool context.
    //
    void
    enqueue_io_task(
        schemas::object_request&& object_request,
        std::shared_ptr<container> container,
        network::server_response_callback&& response_callback) override;

    //
    // Waits for the read dispatcher thread pool
    // to finish execution in a blocking manner.
    //
    void
    wait_for_stop() override;

private:

    //
    // Read IO requests dispatcher entry point.
    // This is the upcall entry point for the thread pool.
    //
    void
    dispatch_read_io_operation(
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
    // Executes a get operation with the storage engine.
    //
    status::status_code
    execute_get_operation(
        storage_engine_reference_handle* container_storage_engine_reference,
        const schemas::object_request& object_request,
        byte_stream& object_data);

    //
    // IO thread pool for dispatching read IO storage engine operations.
    //
    boost::asio::thread_pool read_io_thread_pool_;

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