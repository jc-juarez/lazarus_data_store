// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Main
// 'lazarus_data_store.cc'
// Author: jcjuarez
// Description:
//      Lazarus data store root object. 
// ****************************************************

#include <csignal>
#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include "../common/aliases.hh"
#include "lazarus_data_store.hh"
#include "../network/server/server.hh"
#include "../storage/storage_engine.hh"
#include "../storage/container_index.hh"
#include "../storage/frontline_cache.hh"
#include "../common/args_validations.hh"
#include "../storage/garbage_collector.hh"
#include "../storage/read_io_dispatcher.hh"
#include "../storage/write_io_dispatcher.hh"
#include "../common/system_configuration.hh"
#include <spdlog/sinks/rotating_file_sink.h>
#include "../storage/object_management_service.hh"
#include "../storage/orphaned_container_scavenger.hh"
#include "../storage/container_management_service.hh"
#include "../storage/container_operation_serializer.hh"
#include "../network/server/request-handlers/object/get_object_request_handler.hh"
#include "../network/server/request-handlers/object/insert_object_request_handler.hh"
#include "../network/server/request-handlers/object/remove_object_request_handler.hh"
#include "../network/server/request-handlers/container/create_container_request_handler.hh"
#include "../network/server/request-handlers/container/remove_container_request_handler.hh"

namespace lazarus
{

std::stop_source lazarus::lazarus_data_store::stop_source_;

lazarus_data_store::lazarus_data_store(
    const boost::uuids::uuid session_id,
    const network::server_configuration& server_config,
    const storage::storage_configuration& storage_configuration)
    : session_id_{session_id}
{
    construct_dependency_tree(
        server_config,
        storage_configuration);
}

exit_code
lazarus_data_store::run(
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

        //
        // Register termination signals to be handled by the system.
        //
        register_signals();

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

        //
        // Initialize the logger to be used by the system.
        //
        initialize_logger(
            session_id,
            system_config.logger_configuration_);

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

std::stop_token
lazarus_data_store::get_stop_source_token()
{
    return stop_source_.get_token();
}

void
lazarus_data_store::construct_dependency_tree(
    const network::server_configuration& server_config,
    const storage::storage_configuration& storage_configuration)
{
    //
    // Storage engine component allocation.
    //
    storage_engine_ = std::make_shared<storage::storage_engine>(
        storage_configuration);

    //
    // Object container index component allocation.
    //
    container_index_ = std::make_shared<storage::container_index>(
        storage_configuration.container_index_number_buckets_,
        storage_engine_);

    //
    // Orphaned container scavenger component allocation.
    //
    auto orphaned_container_scavenger = std::make_unique<storage::orphaned_container_scavenger>(
        storage_engine_,
        container_index_);

    //
    // Garbage collector component allocation.
    //
    garbage_collector_ = std::make_unique<storage::garbage_collector>(
        storage_configuration,
        container_index_,
        std::move(orphaned_container_scavenger));

    //
    // Object container operation serializer component allocation.
    //
    auto container_operation_serializer = std::make_unique<storage::container_operation_serializer>(
        storage_engine_,
        container_index_);

    //
    // Object container management service component allocation.
    //
    container_management_service_ = std::make_shared<storage::container_management_service>(
        storage_configuration,
        storage_engine_,
        container_index_,
        std::move(container_operation_serializer));

    //
    // Frontline cache component allocation.
    //
    frontline_cache_ = std::make_shared<storage::frontline_cache>(
        storage_configuration.number_frontline_cache_shards_,
        storage_configuration.max_frontline_cache_shard_size_mib_ * 1'024 * 1'024,
        storage_configuration.max_frontline_cache_shard_object_size_bytes,
        container_index_);

    //
    // Write request dispatcher component allocation.
    //
    write_request_dispatcher_ = std::make_shared<storage::write_io_dispatcher>(
        storage_configuration.number_write_io_threads_,
        storage_engine_,
        frontline_cache_);

    //
    // Read request dispatcher component allocation.
    //
    read_request_dispatcher_ = std::make_shared<storage::read_io_dispatcher>(
        storage_configuration.number_read_io_threads_,
        storage_engine_,
        frontline_cache_);

    //
    // Object management service component allocation.
    //
    object_management_service_ = std::make_shared<storage::object_management_service>(
        storage_configuration,
        container_index_,
        write_request_dispatcher_,
        read_request_dispatcher_,
        frontline_cache_);

    //
    // Create container request handler allocation.
    //
    auto create_container_request_handler = std::make_unique<network::create_container_request_handler>(
        container_management_service_);

    //
    // Remove container request handler allocation.
    //
    auto remove_container_request_handler = std::make_unique<network::remove_container_request_handler>(
        container_management_service_);

    //
    // Insert object request handler allocation
    //
    auto insert_object_request_handler = std::make_unique<network::insert_object_request_handler>(
        object_management_service_);

    //
    // Get object request handler allocation
    //
    auto get_object_request_handler = std::make_unique<network::get_object_request_handler>(
        object_management_service_);

    //
    // Remove object request handler allocation
    //
    auto remove_object_request_handler = std::make_unique<network::remove_object_request_handler>(
        object_management_service_);

    //
    // Server component allocation.
    //
    server_ = std::make_shared<network::server>(
        server_config,
        std::move(create_container_request_handler),
        std::move(remove_container_request_handler),
        std::move(insert_object_request_handler),
        std::move(get_object_request_handler),
        std::move(remove_object_request_handler));
}

status::status_code
lazarus_data_store::start_data_store() const
{
    //
    // Before starting the server, fetch all persistent object containers from the 
    // filesystem. These are needed for starting the storage engine so that it can
    // later associate an object container name to its respective column family reference.
    // This is a static invocation being executed before the storage engine is started.
    //
    std::vector<std::string> containers_names;
    status::status_code status = storage_engine_->fetch_containers_from_disk(
        &containers_names);

    if (status::failed(status))
    {
        spdlog::critical("Failed to fetch the initial object containers from disk during the system startup. "
            "Status={}.",
            status);

        return status;
    }

    //
    // Start the storage engine. On success, it will associate the object
    // containers names to their respective column family reference.
    //
    std::unordered_map<std::string, storage::storage_engine_reference_handle*> storage_engine_references_mapping;
    status = storage_engine_->start(
        containers_names,
        &storage_engine_references_mapping);

    if (status::failed(status))
    {
        spdlog::critical("Failed to start the storage engine during the system startup. "
            "Status={}.",
            status);

        return status;
    }

    //
    // Populate the in-memory object container index with the
    // references obtained when the storage engine was started.
    //
    container_management_service_->populate_container_index(
        &storage_engine_references_mapping);

    if (status::failed(status))
    {
        spdlog::critical("Failed to populate the object container index during the system startup. "
            "Status={}.",
            status);

        return status;
    }

    //
    // Start the system garbage collector.
    // This needs to be started after all initial orphaned
    // object containers are discovered for proper initial cleanup.
    //
    garbage_collector_->start();

    //
    // Start the server for handling incoming data requests.
    // This will block the main thread.
    //
    server_->start();

    return status::success;
}

void
lazarus_data_store::initialize_logger(
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

void
lazarus_data_store::register_signals()
{
    //
    // The system handles 'Ctrl-C' and 'kill' commands by itself.
    //
    std::signal(SIGINT, &lazarus_data_store::signal_handler);
    std::signal(SIGTERM, &lazarus_data_store::signal_handler);
}

void
lazarus_data_store::signal_handler(
    std::int32_t signal)
{
    spdlog::info("Termination signal received. Requesting system stop.");
    network::server::stop();
    stop_source_.request_stop();
}

} // namespace lazarus.