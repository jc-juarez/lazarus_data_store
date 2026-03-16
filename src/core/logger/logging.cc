// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Logger
// 'logging.cc'
// Author: jcjuarez
// Description:
//      Logging utilities for the system.
// ****************************************************

#include "logging.hh"
#include <filesystem>

namespace pandora
{
namespace logger
{

thread_local std::string context::request_id;

void
init_logger(
    const boost::uuids::uuid session_id,
    const logger_configuration& logger_config)
{
    spdlog::init_thread_pool(
        logger_config.queue_size_bytes_,
        1u /* thread_count */);

    const std::string current_session_logs_directory =
        logger_config.logging_session_directory_prefix_
        + "-" + generate_log_directory_time_prefix()
        + "-" + common::uuid_to_string(session_id);
    const std::filesystem::path logging_session_directory_path =
        std::filesystem::path(logger_config.logs_directory_path_) / current_session_logs_directory / logger_config.log_file_prefix_;

    auto logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
        logger_config.component_name_,
        logging_session_directory_path.string(),
        logger_config.max_log_file_size_bytes_,
        logger_config.max_number_files_for_session_);

    spdlog::set_default_logger(logger);
    spdlog::flush_on(spdlog::level::critical);
    spdlog::flush_every(std::chrono::milliseconds(logger_config.flush_frequency_ms_));

    TRACE_LOG(info, "Logger has been initialized successfully. "
        "LogsDirectoryPath={}, "
        "ComponentName={}, "
        "QueueSizeBytes={}, "
        "MaxLogFileSizeBytes={}, "
        "MaxNumberFilesForSession={}, "
        "FlushFrequencyMs={}, "
        "LogFilePrefix={}, "
        "LoggingSessionDirectoryPrefix={}.",
        logger_config.logs_directory_path_ ,
        logger_config.component_name_ ,
        logger_config.queue_size_bytes_ ,
        logger_config.max_log_file_size_bytes_ ,
        logger_config.max_number_files_for_session_ ,
        logger_config.flush_frequency_ms_ ,
        logger_config.log_file_prefix_ ,
        logger_config.logging_session_directory_prefix_);
}

std::string
generate_log_directory_time_prefix()
{
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::floor<std::chrono::minutes>(now);
    return std::format("{:%Y_%m_%d_%Hh_%Mm}", now_time);
}

} // namespace logger.
} // namespace pandora.