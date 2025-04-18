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
#include <filesystem>

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
    {
        //
        // Set the server logs path with the default home directory path if no path
        // override was specified. Throws if fails to obtain the home environment variable.
        //
        const char* home_environment_variable = std::getenv("HOME");

        if (home_environment_variable == nullptr)
        {
            throw std::runtime_error("Failed to access the HOME environment "
                "variable for setting the server logs path.");
        }

        server_logs_directory_path_ =
            std::string(home_environment_variable) + "/lazarus/server-logs";

        //
        // Create the path if it does not exist.
        //
        std::filesystem::create_directories(server_logs_directory_path_);
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

    //
    // Run the lazarus server as daemon.
    //
    bool run_as_daemon_;
};

} // namespace network.
} // namespace lazarus.