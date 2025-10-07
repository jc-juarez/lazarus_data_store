// ****************************************************
// Lazarus Data Store
// Common
// 'system_configuration.hh'
// Author: jcjuarez
// Description:
//      Container for storing all system
//      configurations.
// ****************************************************

#pragma once

#include "nlohmann/json.hpp"
#include "../logger/logger_configuration.hh"
#include "../storage/storage_configuration.hh"
#include "../network/server/server_configuration.hh"

namespace lazarus
{
namespace common
{

class system_configuration
{
public:

    //
    // Constructor.
    //
    system_configuration() = default;

    //
    // Loads the configurations from a config file.
    //
    void
    load_configuration_from_file(
        const std::string& config_file_path);

    //
    // Creates the system directories required for the data store to start.
    //
    void
    set_up_system_directories();

    //
    // Configurations for the logger components.
    //
    logger::logger_configuration logger_configuration_;

    //
    // Configurations for the storage components.
    //
    storage::storage_configuration storage_configuration_;

    //
    // Configurations for the server components.
    //
    network::server_configuration server_configuration_;

private:

    //
    // Loads the logger configurations from a file contents.
    //
    void
    load_logger_configuration(
        const nlohmann::json& config_section);

    //
    // Loads the storage configurations from a file contents.
    //
    void
    load_storage_configuration(
        const nlohmann::json& config_section);

    //
    // Loads the server configurations from a file contents.
    //
    void
    load_server_configuration(
        const nlohmann::json& config_section);

    //
    // Logger file section name.
    //
    static constexpr std::string_view logger_config_section_name_ = "logger";

    //
    // Storage file section name.
    //
    static constexpr std::string_view storage_config_section_name_ = "storage";

    //
    // Server file section name.
    //
    static constexpr std::string_view server_config_section_name_ = "server";
};

} // namespace common.
} // namespace lazarus.