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
#include "server_configuration.hh"

namespace lazarus
{
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
        const server_configuration& server_config);

    //
    // Starts the HTTP server for processing storage requests.
    // This may or may not block the calling thread depending if the daemon option was specified.
    //
    void
    start();

    //
    // Registers a new endpoint with the server.
    //
    template <typename T>
    void
    register_endpoint(
        std::shared_ptr<T> endpoint)
    {
        http_server_.registerController(endpoint);
    }
        
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