// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'concurrent_io_dispatcher.hh'
// Author: jcjuarez
// Description:
//      Thread pool based dispatcher
//      for storage IO operations.
// ****************************************************

#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "../network/server/server.hh"
#include "../schemas/request-interfaces/object_request.hh"

namespace lazarus::storage
{

class container;
class frontline_cache;
class storage_engine_interface;

class concurrent_io_dispatcher
{
public:

    //
    // Constructor.
    //
    concurrent_io_dispatcher(
        const std::uint32_t number_threads,
        std::shared_ptr<storage_engine_interface> storage_engine,
        std::shared_ptr<storage::frontline_cache> frontline_cache);

    //
    // Enqueues a concurrent IO operation for the storage engine.
    // Derived implementations must provide a response back to the server through the async callback.
    //
    void
    enqueue_concurrent_io_request(
        schemas::object_request&& object_request,
        std::shared_ptr<container> container,
        network::server_response_callback&& response_callback);

protected:

    //
    // IO requests dispatcher entry point.
    // Must be implemented by derived classes.
    //
    virtual
    void
    concurrent_io_request_proxy(
        schemas::object_request&& object_request,
        std::shared_ptr<container> container,
        network::server_response_callback&& response_callback) = 0;

    //
    // Handles the insertion of an element into the frontline cache.
    // Must be implemented by derived classes.
    //
    virtual
    void
    insert_object_into_cache(
        schemas::object_request& object_request) = 0;

    //
    // Reference for the storage engine component.
    //
    std::shared_ptr<storage_engine_interface> storage_engine_;

    //
    // IO thread pool for dispatching storage engine operations.
    //
    boost::asio::thread_pool io_thread_pool_;

    //
    // Frontline cache handle.
    //
    std::shared_ptr<storage::frontline_cache> frontline_cache_;
};

} // namespace lazarus::common.