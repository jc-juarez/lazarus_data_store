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
#include "../endpoints/object_container_endpoint.hh"

namespace lazarus
{
namespace network
{

server::server(
    const server_configuration& server_config)
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
    register_endpoints();
}

void
server::start()
{
    spdlog::info("Starting lazarus data store server on port={}.",
        server_config_.port_number_);

    http_server_.run();
}

void
server::register_endpoints()
{
    //
    // Object container endpoint.
    //
    http_server_.registerController(std::make_shared<object_container_endpoint>("Hello"));
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

} // namespace network.
} // namespace lazarus.