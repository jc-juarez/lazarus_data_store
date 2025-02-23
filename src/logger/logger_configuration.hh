// ****************************************************
// Lazarus Data Store
// Logger
// 'logger_configuration.hh'
// Author: jcjuarez
// Description:
//      Container that stores the default
//      configurations for the system-wide logger.
// ****************************************************

#pragma once

#include <filesystem>

namespace lazarus
{
namespace logger
{

//
// Logger configuration container.
//
struct logger_configuration
{
    //
    // Constructor for the logger configurations.
    // Specifies the default values to be used by the logger.
    //
    logger_configuration()
        : logs_directory_path_{std::filesystem::current_path()},
          component_name_{"LazarusDataStore"},
          queue_size_bytes_{32 * 1'024u},
          max_log_file_size_bytes_{10u * 1'024u * 1'024u},
          max_number_files_for_session_{100u},
          flush_frequency_ms_{1'000u},
          log_file_prefix_{"lazarus_ds.log"},
          logging_session_directory_prefix_{"lazarus-logs"}
    {}

    //
    // Path to the directory where the logging session directory will be created.
    //
    std::filesystem::path logs_directory_path_;

    //
    // Component name to be used by the logger for service identification.
    //
    std::string component_name_;

    //
    // Queue size for the logger in bytes.
    //
    std::uint32_t queue_size_bytes_;

    //
    // Max size of each log file in bytes.
    //
    std::uint32_t max_log_file_size_bytes_;

    //
    // Max number of files associated with the session (retains the latest X files).
    //
    std::uint32_t max_number_files_for_session_;

    //
    // Disk flush frequency in milliseconds.
    //
    std::uint32_t flush_frequency_ms_;

    //
    // Log files prefix.
    //
    std::string log_file_prefix_;

    //
    // Session log directory prefix.
    //
    std::string logging_session_directory_prefix_;
};

} // namespace logger.
} // namespace lazarus.