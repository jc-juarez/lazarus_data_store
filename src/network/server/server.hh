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

#include <unordered_map>
#include <drogon/drogon.h>
#include "../../status/status.hh"
#include "../../common/aliases.hh"
#include "server_configuration.hh"

namespace lazarus
{

namespace storage
{
    class object_container_management_service;
    class object_management_service;
}

namespace network
{

//
// Main HTTP server wrapper.
// This class does not own the internal HTTP server handle as it is a global access singleton.
// Instead, it provides a concise set of APIs for safe access and configurations containerization.
//
class server
{
public:

    //
    // Constructor for the HTTP server.
    //
    server(
        const server_configuration& server_config,
        std::shared_ptr<storage::object_container_management_service> object_container_management_service_handle,
        std::shared_ptr<storage::object_management_service> object_management_service);

    //
    // Starts the HTTP server for processing storage requests.
    // This may or may not block the calling thread depending if the daemon option was specified.
    //
    void
    start();

    //
    // Stops the HTTP server. Handles requests handling termination.
    // This will exit the event loop and terminate all network IO.
    //
    static
    void
    stop();

    //
    // Register the endpoints that the server needs to handle.
    //
    void
    register_endpoints(
        std::shared_ptr<storage::object_container_management_service> object_container_management_service_handle,
        std::shared_ptr<storage::object_management_service> object_management_service);
        
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
    // Sends back a response to a client over a provided response_callback.
    //
    static
    void
    send_response(
        const server_response_callback& response_callback,
        const status::status_code& status,
        std::unordered_map<std::string, std::string>* additional_parameters = nullptr);

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