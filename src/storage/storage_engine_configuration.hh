// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_engine_configuration.hh'
// Author: jcjuarez
// Description:
//      Container that stores the default
//      configurations for the storage engine.
// ****************************************************

#pragma once

#include <string>
#include <cstdlib>
#include <stdexcept>

namespace lazarus
{
namespace storage
{

//
// Storage engine configuration container.
//
struct storage_engine_configuration
{
    //
    // Constructor for the storage engine configurations.
    // Specifies the default values to be used by the storage engine.
    //
    storage_engine_configuration()
        : core_database_path_{""}
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
            std::string(home_environment_variable) + "/lazarus-ds";
    }

    //
    // Directory path for the core database.
    //
    std::string core_database_path_;
};

} // namespace storage.
} // namespace lazarus.