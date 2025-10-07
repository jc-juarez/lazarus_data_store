// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
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
          server_number_threads_{16u},
          server_listener_ip_address_{"0.0.0.0"}
    {
        //
        // Set the server logs path with the default home directory path.
        // Throws if fails to obtain the home environment variable.
        //
        const char* home_environment_variable = std::getenv("HOME");

        if (home_environment_variable == nullptr)
        {
            throw std::runtime_error("Failed to access the HOME environment "
                "variable for setting the server logs path.");
        }

        server_logs_directory_path_ =
            std::string(home_environment_variable) + "/lazarus/server-logs";
    }

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
};

} // namespace network.
} // namespace lazarus.