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
#include <drogon/drogon.h>
#include "../common/aliases.hh"
#include "lazarus_data_store.hh"
#include "../network/server/server.hh"
#include "../storage/storage_engine.hh"
#include "../storage/container_index.hh"
#include "../storage/garbage_collector.hh"
#include "../storage/read_io_dispatcher.hh"
#include "../storage/write_io_dispatcher.hh"
#include <spdlog/sinks/rotating_file_sink.h>
#include "../storage/object_management_service.hh"
#include "../network/server/server_configuration.hh"
#include "../storage/orphaned_container_scavenger.hh"
#include "../storage/container_management_service.hh"
#include "../storage/container_operation_serializer.hh"

namespace lazarus
{

std::stop_source lazarus::lazarus_data_store::stop_source_;

lazarus_data_store::lazarus_data_store(
    const boost::uuids::uuid session_id,
    const network::server_configuration& server_config,
    const storage::storage_configuration& storage_configuration)
    : session_id_{session_id}
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
    // Write request dispatcher component allocation.
    //
    write_request_dispatcher_ = std::make_shared<storage::write_io_dispatcher>(
        storage_configuration.number_write_io_threads_,
        storage_engine_);

    //
    // Read request dispatcher component allocation.
    //
    read_request_dispatcher_ = std::make_shared<storage::read_io_dispatcher>(
        storage_configuration.number_read_io_threads_,
        storage_engine_);

    //
    // Object management service component allocation.
    //
    object_management_service_ = std::make_shared<storage::object_management_service>(
        storage_configuration,
        container_index_,
        write_request_dispatcher_,
        read_request_dispatcher_);

    //
    // Server component allocation.
    //
    server_ = std::make_shared<network::server>(
        server_config,
        container_management_service_,
        object_management_service_);
}

exit_code
lazarus_data_store::run()
{
    status::status_code status = status::success;
    const boost::uuids::uuid session_id = common::generate_uuid();

    //
    // Register termination signals to be handled by the system.
    //
    register_signals();

    //
    // Initialize the logger to be used by the system.
    // This must be placed outside the root exception handler for
    // ensuring the system throws with an unhandled exception if initialization
    // fails given that the system must not start without the logger enabled.
    //
    initialize_logger(
        session_id,
        lazarus::logger::logger_configuration{});

    try
    {
        //
        // Initialize all core dependencies of the data store.
        //
        lazarus::lazarus_data_store lazarus_ds{
            session_id,
            lazarus::network::server_configuration{},
            lazarus::storage::storage_configuration{}};

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

status::status_code
lazarus_data_store::start_data_store() const {
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