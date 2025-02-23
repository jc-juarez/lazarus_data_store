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

namespace lazarus
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
}

void
server::start()
{
    http_server_.run();
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

} // namespace lazarus.