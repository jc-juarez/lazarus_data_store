// ****************************************************
// Lazarus Data Store
// Storage
// 'write_request_dispatcher.hh'
// Author: jcjuarez
// Description:
//      Write request dispatcher module for IO.
//      Implemented as a thread pool dispatcher.
// ****************************************************

#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "../network/server/server.hh"
#include "../schemas/request-interfaces/object_request_interface.hh"

namespace lazarus::storage
{

class storage_engine;
class object_container;

class write_request_dispatcher
{
public:

    //
    // Constructor.
    //
    write_request_dispatcher(
        const std::uint32_t number_write_io_threads,
        std::shared_ptr<storage_engine> storage_engine);

    //
    // Enqueues a concurrent write IO operation for the storage engine.
    // Provides a response back to the server through the async callback.
    //
    void
    enqueue_write_request(
        schemas::object_request_interface&& object_request,
        std::shared_ptr<object_container> object_container,
        network::server_response_callback&& response_callback);

private:

    //
    // Write IO requests dispatcher entry point.
    //
    void
    write_request_concurrent_proxy(
        schemas::object_request_interface&& object_request,
        std::shared_ptr<object_container> object_container,
        network::server_response_callback&& response_callback);

    status::status_code
    execute_insert_operation(
        storage_engine_reference_handle* object_container_storage_engine_reference,
        const schemas::object_request_interface& object_request);

    status::status_code
    execute_remove_operation(
        storage_engine_reference_handle* object_container_storage_engine_reference,
        const schemas::object_request_interface& object_request);

    //
    // Reference for the storage engine component.
    //
    std::shared_ptr<storage_engine> storage_engine_;

    //
    // Write IO thread pool for dispatching storage engine operations.
    //
    boost::asio::thread_pool write_io_thread_pool_;
};

} // namespace lazarus::common.