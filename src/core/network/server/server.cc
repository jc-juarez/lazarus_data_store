// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'server.cc'
// Author: jcjuarez
// Description:
//      Main HTTP server wrapper for handling
//      incoming storage processing requests. 
// ****************************************************

#include "server.hh"
#include <spdlog/spdlog.h>
#include "../endpoints/ping.hh"
#include "../endpoints/objects.hh"
#include "../endpoints/containers.hh"
#include "../../storage/cache/frontline_cache.hh"
#include "../../common/response_utilities.hh"
#include "request-handlers/object/get_object_request_handler.hh"
#include "request-handlers/object/insert_object_request_handler.hh"
#include "request-handlers/object/remove_object_request_handler.hh"
#include "request-handlers/container/create_container_request_handler.hh"
#include "request-handlers/container/remove_container_request_handler.hh"

namespace lazarus
{
namespace network
{

server::server(
    const server_configuration& server_config,
    std::unique_ptr<create_container_request_handler> create_container_request_handler,
    std::unique_ptr<remove_container_request_handler> remove_container_request_handler,
    std::unique_ptr<insert_object_request_handler> insert_object_request_handler,
    std::unique_ptr<get_object_request_handler> get_object_request_handler,
    std::unique_ptr<remove_object_request_handler> remove_object_request_handler)
    : http_server_{drogon::app()},
      server_config_{server_config}
{
    http_server_.setLogPath(server_config_.server_logs_directory_path_)
         .setLogLevel(trantor::Logger::kInfo)
         .addListener(server_config_.server_listener_ip_address_, server_config_.port_number_)
         .setThreadNum(server_config_.server_number_threads_)
         .disableSigtermHandling();

    //
    // Register all needed endpoints for the server
    // along with their required request handlers.
    //
    register_endpoints(
        std::move(create_container_request_handler),
        std::move(remove_container_request_handler),
        std::move(insert_object_request_handler),
        std::move(get_object_request_handler),
        std::move(remove_object_request_handler));
}

void
server::start()
{
    spdlog::info("Starting lazarus data store server. "
        "Port={}, "
        "LogsDirectoryPath={}, "
        "NumThreads={}, "
        "ListenerIpAddress={}.",
        server_config_.port_number_,
        server_config_.server_logs_directory_path_,
        server_config_.server_number_threads_,
        server_config_.server_listener_ip_address_);

    http_server_.run();
}

void
server::stop()
{
    spdlog::info("Stopping lazarus data store server.");

    drogon::app().quit();
}

void
server::register_endpoints(
    std::unique_ptr<create_container_request_handler> create_container_request_handler,
    std::unique_ptr<remove_container_request_handler> remove_container_request_handler,
    std::unique_ptr<insert_object_request_handler> insert_object_request_handler,
    std::unique_ptr<get_object_request_handler> get_object_request_handler,
    std::unique_ptr<remove_object_request_handler> remove_object_request_handler)
{
    //
    // Container endpoint along its request handlers.
    //
    http_server_.registerController(std::make_shared<endpoints::containers>(
        std::move(create_container_request_handler),
        std::move(remove_container_request_handler)));

    //
    // Object endpoint along its request handlers.
    //
    http_server_.registerController(std::make_shared<endpoints::objects>(
        std::move(insert_object_request_handler),
        std::move(get_object_request_handler),
        std::move(remove_object_request_handler)));

    //
    // Ping endpoint for liveliness probes.
    //
    http_server_.registerController(std::make_shared<endpoints::ping>());
}

void
server::send_response(
    const server_response_callback& response_callback,
    const status::status_code& status,
    response_fields* response_fields)
{
    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(static_cast<drogon::HttpStatusCode>(status.get_http_status_code()));
    response->setBody(common::response_utilities::generate_server_json_response(status.get_internal_status_code(), response_fields));
    response_callback(response);
}

} // namespace network.
} // namespace lazarus.