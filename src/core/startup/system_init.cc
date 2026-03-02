// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'collocation_builder.cc'
// Author: jcjuarez
// Description:
//      Contains the system initialization logic.
// ****************************************************

#include <csignal>
#include "system_init.hh"
#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include "lazarus_data_store.hh"
#include "../network/server/server.hh"
#include "../common/args_validations.hh"
#include <spdlog/sinks/rotating_file_sink.h>

namespace lazarus
{

exit_code
init_system(
    const std::vector<std::string>& args)
{
    status::status_code status = status::success;

    try
    {
        //
        // Assign the session ID for the process-lifetime.
        // This ID is used for logging correlation.
        //
        const boost::uuids::uuid session_id = common::generate_uuid();
        const auto system_config = generate_system_configs(args);

        //
        // Initial all global dependencies for the system.
        //
        init_global_dependencies(
            session_id,
            system_config);

        //
        // Initialize all core dependencies of the data store.
        //
        lazarus::lazarus_data_store lazarus_ds{
            session_id,
            system_config.server_configuration_,
            system_config.storage_configuration_};

        //
        // Start the data store system. This will start the core
        // storage engine and the main server for handling data requests.
        //
        status = lazarus_ds.start_data_store();
    }
    catch (const std::exception& exception)
    {
        //
        // Generic operation failed and threw.
        // Handle the error gracefully and terminate the system.
        //
        status = status::fail;

        spdlog::critical("Exception thrown in the data store startup path. Terminating the data store. "
            "Exception={}",
            exception.what());
    }

    return status::succeeded(status) ? EXIT_SUCCESS : EXIT_FAILURE;
}

void
init_global_dependencies(
    const boost::uuids::uuid session_id,
    const common::system_configuration& system_config)
{
    //
    // Register termination signals to be handled by the system.
    //
    register_signals();

    //
    // Initialize the logger to be used by the system.
    //
    init_logger(
        session_id,
        system_config.logger_configuration_);
}

void
init_logger(
    const boost::uuids::uuid session_id,
    const logger::logger_configuration logger_config)
{
    spdlog::init_thread_pool(
        logger_config.queue_size_bytes_,
        1u /* thread_count */);

    const std::string current_session_logs_directory =
    logger_config.logging_session_directory_prefix_ + "-" + common::uuid_to_string(session_id);
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

    spdlog::info("Logger has been initialized successfully. "
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

common::system_configuration
generate_system_configs(
    const std::vector<std::string>& args)
{
    //
    // Validate the args provided by the process invoker.
    // At the point the logger has not been yet initialized,
    // so the function below can throw for debuggability purposes.
    //
    common::validate_process_args(args);

    //
    // Instantiate the configurations to be used by the system
    // and load the configurations from a config file if available.
    //
    common::system_configuration system_config;

    if (args.size() == common::max_args_count)
    {
        //
        // This implies a config file was provided, so override
        // the default configurations with the ones provided in the file.
        //
        system_config.load_configuration_from_file(args.back());
    }

    //
    // Create the required system directories which need to be present
    // before spinning up the rest of the system.
    //
    system_config.set_up_system_directories();

    return system_config;
}

std::stop_token
get_stop_source_token()
{
    return stop_source.get_token();
}

void
register_signals()
{
    //
    // The system handles 'Ctrl-C' and 'kill' commands by itself.
    //
    std::signal(SIGINT, &signal_handler);
    std::signal(SIGTERM, &signal_handler);
}

void
signal_handler(
    std::int32_t signal)
{
    spdlog::info("Termination signal received. Requesting system stop.");
    network::server::stop();
    stop_source.request_stop();
}

} // namespace lazarus.