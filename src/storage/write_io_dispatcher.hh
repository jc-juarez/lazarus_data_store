// ****************************************************
// Lazarus Data Store
// Storage
// 'write_io_dispatcher.hh'
// Author: jcjuarez
// Description:
//      Write request dispatcher module for IO.
//      Implemented as a thread pool dispatcher.
// ****************************************************

#pragma once

#include "concurrent_io_dispatcher.hh"
#include "../network/server/server.hh"
#include "../schemas/request-interfaces/object_request.hh"

namespace lazarus::storage
{

class write_io_dispatcher : public concurrent_io_dispatcher
{
public:

    //
    // Constructor.
    //
    write_io_dispatcher(
        const std::uint32_t number_write_io_threads,
        std::shared_ptr<storage_engine> storage_engine,
        std::shared_ptr<storage::frontline_cache> frontline_cache);

private:

    //
    // Write IO requests dispatcher entry point.
    //
    void
    concurrent_io_request_proxy(
        schemas::object_request&& object_request,
        std::shared_ptr<container> container,
        network::server_response_callback&& response_callback) override;

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
};

} // namespace lazarus::common.