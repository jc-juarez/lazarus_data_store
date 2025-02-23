// ****************************************************
// Lazarus Data Store
// Network
// 'server_configuration.hh'
// Author: jcjuarez
// Description:
//      Container that stores the default
//      configurations for the main HTTP server.
// ****************************************************

#pragma once

#include <string>
#include <cstdint>

namespace lazarus
{
namespace network
{

//
// Sever configuration container.
//
struct server_configuration
{
    //
    // Constructor for the server configurations.
    // Specifies the default values to be used by the server.
    //
    server_configuration()
        : port_number_{8080},
          server_logs_directory_path_{"./"},
          server_number_threads_{16u},
          server_listener_ip_address_{"0.0.0.0"},
          run_as_daemon_{false}
    {}

    //
    // Port number for the HTTP server.
    //
    std::uint16_t port_number_;

    //
    // Directory path for the HTTP server logs.
    //
    std::string server_logs_directory_path_;

    //
    // Number of threads for the HTTP server.
    //
    std::uint16_t server_number_threads_;

    //
    // HTTP server listener IP address.
    //
    std::string server_listener_ip_address_;

    //
    // Run the lazarus server as daemon.
    //
    bool run_as_daemon_;
};

} // namespace network.
} // namespace lazarus.