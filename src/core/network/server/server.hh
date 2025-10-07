// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
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
namespace network
{

class create_container_request_handler;
class remove_container_request_handler;
class insert_object_request_handler;
class get_object_request_handler;
class remove_object_request_handler;

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
        std::unique_ptr<create_container_request_handler> create_container_request_handler,
        std::unique_ptr<remove_container_request_handler> remove_container_request_handler,
        std::unique_ptr<insert_object_request_handler> insert_object_request_handler,
        std::unique_ptr<get_object_request_handler> get_object_request_handler,
        std::unique_ptr<remove_object_request_handler> remove_object_request_handler);

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
        std::unique_ptr<create_container_request_handler> create_container_request_handler,
        std::unique_ptr<remove_container_request_handler> remove_container_request_handler,
        std::unique_ptr<insert_object_request_handler> insert_object_request_handler,
        std::unique_ptr<get_object_request_handler> get_object_request_handler,
        std::unique_ptr<remove_object_request_handler> remove_object_request_handler);

    //
    // Sends back a response to a client over a provided response_callback.
    //
    static
    void
    send_response(
        const server_response_callback& response_callback,
        const status::status_code& status,
        response_fields* response_fields = nullptr);

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