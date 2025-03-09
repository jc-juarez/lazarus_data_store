// ****************************************************
// Lazarus Data Store
// Network
// 'server.hh'
// Author: jcjuarez
// Description:
//      Main HTTP server wrapper for handling
//      incoming storage processing requests. 
// ****************************************************

#pragma once

#include <drogon/drogon.h>
#include "../../status/status.hh"
#include "server_configuration.hh"

namespace lazarus
{

namespace storage
{
    class data_store_service;
}

namespace network
{

//
// Main HTTP server wrapper.
//
class server
{
public:

    //
    // Constructor for the HTTP server.
    //
    server(
        const server_configuration& server_config,
        std::shared_ptr<lazarus::storage::data_store_service> data_store_service_handle);

    //
    // Starts the HTTP server for processing storage requests.
    // This may or may not block the calling thread depending if the daemon option was specified.
    //
    void
    start();

    //
    // Register the endpoints that the server needs to handle.
    //
    void
    register_endpoints(
        std::shared_ptr<lazarus::storage::data_store_service> data_store_service_handle);
        
    //
    // Gets the server associated port number.
    //
    std::uint16_t
    get_port_number() const;

    //
    // Gets the server associated logs directory path.
    //
    const char*
    get_server_logs_directory_path() const;

    //
    // Gets the number of threads used by the HTTP server.
    //
    std::uint16_t
    get_server_number_threads() const;

    //
    // Gets the IP address on which the HTTP server listens to incoming requests.
    //
    const char*
    get_server_listener_ip_address() const;

    //
    // Sends back a response to a client over a provided callback.
    //
    static
    void
    send_response(
        std::function<void(const drogon::HttpResponsePtr &)>& response_callback,
        const status::status_code status);

private:

    //
    // Reference to the underlying HTTP server.
    //
    drogon::HttpAppFramework& http_server_;

    //
    // Configuration for the HTTP server.
    //
    const server_configuration server_config_;
};

} // namespace network.
} // namespace lazarus.