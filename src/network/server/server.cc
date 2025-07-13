// ****************************************************
// Lazarus Data Store
// Network
// 'server.cc'
// Author: jcjuarez
// Description:
//      Main HTTP server wrapper for handling
//      incoming storage processing requests. 
// ****************************************************

#include <format>
#include "server.hh"
#include <spdlog/spdlog.h>
#include "../endpoints/object_endpoint.hh"
#include "../../common/response_utilities.hh"
#include "../endpoints/container_endpoint.hh"
#include "../../storage/container_management_service.hh"

namespace lazarus
{
namespace network
{

server::server(
    const server_configuration& server_config,
    std::shared_ptr<storage::container_management_service> container_management_service_handle,
    std::shared_ptr<storage::object_management_service> object_management_service)
    : http_server_{drogon::app()},
      server_config_{server_config}
{
    http_server_.setLogPath(server_config_.server_logs_directory_path_)
         .setLogLevel(trantor::Logger::kWarn)
         .addListener(server_config_.server_listener_ip_address_, server_config_.port_number_)
         .setThreadNum(server_config_.server_number_threads_)
         .disableSigtermHandling();

    if (server_config_.run_as_daemon_)
    {
        http_server_.enableRunAsDaemon();
    }

    //
    // Register all needed endpoints for the server.
    //
    register_endpoints(
        container_management_service_handle,
        object_management_service);
}

void
server::start()
{
    spdlog::info("Starting lazarus data store server on port={}.",
        server_config_.port_number_);

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
    std::shared_ptr<storage::container_management_service> container_management_service_handle,
    std::shared_ptr<storage::object_management_service> object_management_service)
{
    //
    // Object container endpoint.
    //
    http_server_.registerController(std::make_shared<container_endpoint>(
        container_management_service_handle));

    //
    // Object endpoint.
    //
    http_server_.registerController(std::make_shared<object_endpoint>(
        object_management_service));
}

std::uint16_t
server::get_port_number() const
{
    return server_config_.port_number_;
}

const char*
server::get_server_logs_directory_path() const
{
    return server_config_.server_logs_directory_path_.c_str();
}

std::uint16_t
server::get_server_number_threads() const
{
    return server_config_.server_number_threads_;
}

const char*
server::get_server_listener_ip_address() const
{
    return server_config_.server_listener_ip_address_.c_str();
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