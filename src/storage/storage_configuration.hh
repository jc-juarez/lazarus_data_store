// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_configuration.hh'
// Author: jcjuarez
// Description:
//      Container that stores the default
//      configurations for the storage subsystem.
// ****************************************************

#pragma once

#include <string>
#include <cstdlib>
#include <stdexcept>
#include <filesystem>

namespace lazarus
{
namespace storage
{

//
// Storage configurations container.
//
struct storage_configuration
{
    //
    // Constructor for the storage engine configurations.
    // Specifies the default values to be used by the storage engine.
    //
    storage_configuration()
        : core_database_path_{""},
          garbage_collector_periodic_interval_ms_{10'000u}
    {
        //
        // Set the core database path with the default home directory path if no path
        // override was specified. Throws if fails to obtain the home environment variable.
        //
        const char* home_environment_variable = std::getenv("HOME");

        if (home_environment_variable == nullptr)
        {
            throw std::runtime_error("Failed to access the HOME environment "
                "variable for setting the core database path.");
        }

        core_database_path_ =
            std::string(home_environment_variable) + "/lazarus/lazarus-ds";

        //
        // Create the path if it does not exist.
        //
        std::filesystem::create_directories(core_database_path_);
    }

    //
    // Directory path for the core database.
    //
    std::string core_database_path_;

    //
    // Periodic garbage collector interval in milliseconds.
    //
    std::uint32_t garbage_collector_periodic_interval_ms_;
};

} // namespace storage.
} // namespace lazarus.