// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
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
#include "../../network/server/server.hh"
#include "../../schemas/request-interfaces/object_request.hh"

namespace pandora::storage
{

class cache_accessor;
class read_io_executor;

class read_io_dispatcher : public io_dispatcher_interface
{
public:

    //
    // Constructor.
    //
    read_io_dispatcher(
        const std::uint32_t number_read_io_threads,
        std::unique_ptr<read_io_executor> read_io_executor,
        cache_accessor& cache_accessor);

    //
    // Enqueues a read IO operation for to be processed
    // by the dispatcher within a thread pool context.
    //
    void
    enqueue_io_task(
        object_io_task&& read_io_task) override;

    //
    // Waits for the read dispatcher thread pool
    // to finish execution in a blocking manner.
    //
    void
    wait_for_stop() override;

    //
    // Gets the number of read IO threads.
    //
    std::uint32_t
    get_num_io_threads() override;

private:

    //
    // Read IO requests dispatcher entry point.
    // This is the upcall entry point for the thread pool.
    //
    void
    dispatch_read_io_task(
        object_io_task&& read_io_task);

    //
    // Executes a get object task.
    //
    status::status_code
    execute_read_io_task(
        object_io_task& read_io_task,
        byte_stream& object_data);

    //
    // IO thread pool for dispatching read IO storage engine operations.
    //
    boost::asio::thread_pool read_io_thread_pool_;

    //
    // Object IO executor handle.
    //
    std::unique_ptr<read_io_executor> read_io_executor_;

    //
    // Cache accessor handle.
    //
    cache_accessor& cache_accessor_;

    //
    // Number of read IO threads.
    //
    const std::uint32_t num_read_io_threads_;
};

} // namespace pandora::common.