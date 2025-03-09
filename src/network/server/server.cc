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
#include "../../storage/data_store_service.hh"
#include "../endpoints/object_container_endpoint.hh"

namespace lazarus
{
namespace network
{

server::server(
    const server_configuration& server_config,
    std::shared_ptr<storage::data_store_service> data_store_service_handle)
    : http_server_{drogon::app()},
      server_config_{server_config}
{
    http_server_.setLogPath(server_config_.server_logs_directory_path_)
         .setLogLevel(trantor::Logger::kWarn)
         .addListener(server_config_.server_listener_ip_address_, server_config_.port_number_)
         .setThreadNum(server_config_.server_number_threads_);

    if (server_config_.run_as_daemon_)
    {
        http_server_.enableRunAsDaemon();
    }

    //
    // Register all needed endpoints for the server.
    //
    register_endpoints(data_store_service_handle);
}

void
server::start()
{
    spdlog::info("Starting lazarus data store server on port={}.",
        server_config_.port_number_);

    http_server_.run();
}

void
server::register_endpoints(
    std::shared_ptr<storage::data_store_service> data_store_service_handle)
{
    //
    // Object container endpoint.
    //
    http_server_.registerController(std::make_shared<object_container_endpoint>(
        data_store_service_handle));
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
    std::function<void(const drogon::HttpResponsePtr &)>& response_callback,
    const status::status_code status)
{
    const std::string response_body = std::format(
        "{{\n"
        "    \"InternalStatusCode\": {:#X}\n"
        "}}\n", 
        status);

    const drogon::HttpStatusCode http_status_code = status::failed(status) ?
        drogon::HttpStatusCode::k500InternalServerError : drogon::HttpStatusCode::k200OK;

    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(http_status_code);
    response->setBody(response_body);
    response_callback(response);
}

} // namespace network.
} // namespace lazarus.