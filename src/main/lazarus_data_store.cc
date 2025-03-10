// ****************************************************
// Lazarus Data Store
// Main
// 'lazarus_data_store.cc'
// Author: jcjuarez
// Description:
//      Lazarus data store root object. 
// ****************************************************

#include <rocksdb/db.h>
#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <drogon/drogon.h>
#include "lazarus_data_store.hh"
#include "../network/server/server.hh"
#include "../storage/storage_engine.hh"
#include "../storage/data_store_service.hh"
#include <spdlog/sinks/rotating_file_sink.h>
#include "../network/server/server_configuration.hh"

namespace lazarus
{

lazarus_data_store::lazarus_data_store(
    const boost::uuids::uuid session_id,
    const network::server_configuration& server_config,
    const storage::storage_engine_configuration& storage_engine_configuration)
    : session_id_{session_id}
{
    //
    // Storage engine component allocation.
    //
    storage_engine_ = std::make_shared<storage::storage_engine>(
        storage_engine_configuration);

    //
    // Data store service component allocation.
    //
    data_store_service_ = std::make_shared<storage::data_store_service>(
        storage_engine_);

    //
    // Server component allocation.
    //
    server_ = std::make_shared<network::server>(
        server_config,
        data_store_service_);
}

exit_code
lazarus_data_store::run()
{
    status::status_code status = status::success;
    const boost::uuids::uuid session_id = common::generate_uuid();

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
            lazarus::storage::storage_engine_configuration{}};

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

status::status_code
lazarus_data_store::start_data_store()
{
    //
    // Before starting the server, fetch all persistent object containers from the 
    // filesystem. These are needed for starting the storage engine so that it can
    // later associate an object container name to its respective column family reference.
    // This is an static invocation being executed before the storage engine is started.
    //
    std::vector<std::string> object_containers_names;
    status::status_code status = storage_engine_->fetch_object_containers_from_disk(
        &object_containers_names);

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
    std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*> storage_engine_references_mapping;
    storage_engine_->start(
        object_containers_names,
        &storage_engine_references_mapping);

    //
    // Populate the in-memory object container index with the
    // references obtained when the storage engine was started.
    //
    data_store_service_->populate_object_container_index(
        storage_engine_references_mapping);

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
    spdlog::flush_every(std::chrono::milliseconds(logger_config.flush_frequency_ms_));

    spdlog::info("Logger has been initializad successfully. "
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

} // namespace lazarus.
