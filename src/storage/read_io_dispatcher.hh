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

#include "concurrent_io_dispatcher.hh"
#include "../network/server/server.hh"
#include "../schemas/request-interfaces/object_request_interface.hh"

namespace lazarus::storage
{

class read_io_dispatcher : public concurrent_io_dispatcher
{
public:

    //
    // Constructor.
    //
    read_io_dispatcher(
        const std::uint32_t number_read_io_threads,
        std::shared_ptr<storage_engine> storage_engine);

private:

    //
    // Read IO requests dispatcher entry point.
    //
    void
    concurrent_io_request_proxy(
        schemas::object_request_interface&& object_request,
        std::shared_ptr<container> container,
        network::server_response_callback&& response_callback) override;

    //
    // Executes a get operation with the storage engine.
    //
    status::status_code
    execute_get_operation(
        storage_engine_reference_handle* object_container_storage_engine_reference,
        const schemas::object_request_interface& object_request,
        byte_stream& object_data);
};

} // namespace lazarus::common.