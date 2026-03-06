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
#include "collocation_builder.hh"
#include "../network/server/server.hh"
#include "../common/args_validations.hh"
#include <spdlog/sinks/rotating_file_sink.h>
#include "../storage/io/collocation_resolver.hh"

// Remove duplicates.
#include "../storage/io/data_partition.hh"
#include <csignal>
#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include "../common/aliases.hh"
#include "lazarus_data_store.hh"
#include "../network/server/server.hh"
#include "../storage/io/storage_engine.hh"
#include "../storage/index/container_index.hh"
#include "../storage/cache/frontline_cache.hh"
#include "../common/args_validations.hh"
#include "../storage/gc/garbage_collector.hh"
#include "../storage/io/read_io_dispatcher.hh"
#include "../storage/io/write_io_dispatcher.hh"
#include "../storage/io/read_io_executor.hh"
#include "../storage/io/write_batch_aggregator.hh"
#include "../storage/cache/cache_accessor.hh"
#include "../common/system_configuration.hh"
#include <spdlog/sinks/rotating_file_sink.h>
#include "../storage/management/object_management_service.hh"
#include "../storage/gc/orphaned_container_scavenger.hh"
#include "../storage/management/container_management_service.hh"
#include "../storage/management/container_operation_serializer.hh"
#include "../network/server/request-handlers/object/get_object_request_handler.hh"
#include "../network/server/request-handlers/object/insert_object_request_handler.hh"
#include "../network/server/request-handlers/object/remove_object_request_handler.hh"
#include "../network/server/request-handlers/container/create_container_request_handler.hh"
#include "../network/server/request-handlers/container/remove_container_request_handler.hh"

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
        // Start the system.
        //
        status = start_system(
            session_id,
            system_config);
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

status::status_code
start_system(
    const boost::uuids::uuid session_id,
    const common::system_configuration& system_config)
{
    //
    // Construct all the dependencies for the system.
    //
    std::shared_ptr<storage::data_partition> container_metadata_partition;
    std::shared_ptr<storage::collocation_resolver> collocation_resolver;
    std::shared_ptr<storage::data_partition_provider> data_partition_provider;
    std::shared_ptr<storage::threading_context_provider> threading_context_provider;
    std::tie(
        container_metadata_partition,
        collocation_resolver,
        data_partition_provider,
        threading_context_provider) =
            storage::collocation_builder::generate_collocation_topology(system_config.storage_configuration_);

    auto container_index = std::make_shared<storage::container_index>(
        system_config.storage_configuration_.container_index_number_buckets_);

    auto orphaned_container_scavenger = std::make_unique<storage::orphaned_container_scavenger>(
        container_index);

    auto garbage_collector = std::make_unique<storage::garbage_collector>(
        system_config.storage_configuration_,
        container_index,
        std::move(orphaned_container_scavenger));

    auto container_operation_serializer = std::make_unique<storage::container_operation_serializer>(
        container_metadata_partition,
        data_partition_provider,
        container_index);

    auto container_management_service = std::make_shared<storage::container_management_service>(
        system_config.storage_configuration_,
        container_metadata_partition,
        container_index,
        std::move(container_operation_serializer),
        data_partition_provider);

    auto frontline_cache = std::make_shared<storage::frontline_cache>(
        system_config.storage_configuration_.number_frontline_cache_shards_,
        system_config.storage_configuration_.max_frontline_cache_shard_size_mib_ * 1'024 * 1'024,
        system_config.storage_configuration_.max_frontline_cache_shard_object_size_bytes,
        container_index);

    auto cache_accessor = std::make_shared<storage::cache_accessor>(
        frontline_cache);

    auto object_io_executor = std::make_shared<storage::read_io_executor>(
        data_partition_provider);

    auto write_batch_aggregator = std::make_unique<storage::write_batch_aggregator>(
        object_io_executor,
        cache_accessor);

    auto write_io_task_dispatcher = std::make_shared<storage::write_io_dispatcher>(
        std::move(write_batch_aggregator));

    auto read_io_task_dispatcher = std::make_shared<storage::read_io_dispatcher>(
        system_config.storage_configuration_.number_read_io_threads_,
        object_io_executor,
        cache_accessor);

    auto object_management_service = std::make_shared<storage::object_management_service>(
        system_config.storage_configuration_,
        container_index,
        write_io_task_dispatcher,
        read_io_task_dispatcher,
        frontline_cache);

    auto create_container_request_handler = std::make_unique<network::create_container_request_handler>(
        container_management_service);

    auto remove_container_request_handler = std::make_unique<network::remove_container_request_handler>(
        container_management_service);

    auto insert_object_request_handler = std::make_unique<network::insert_object_request_handler>(
        object_management_service);

    auto get_object_request_handler = std::make_unique<network::get_object_request_handler>(
        object_management_service);

    auto remove_object_request_handler = std::make_unique<network::remove_object_request_handler>(
        object_management_service);

    auto server = std::make_shared<network::server>(
        system_config.server_configuration_,
        std::move(create_container_request_handler),
        std::move(remove_container_request_handler),
        std::move(insert_object_request_handler),
        std::move(get_object_request_handler),
        std::move(remove_object_request_handler));

    //
    // Initialize all core dependencies of the data store.
    //
    lazarus_data_store lazarus_ds{
        session_id,
        container_metadata_partition,
        collocation_resolver,
        data_partition_provider,
        threading_context_provider,
        server,
        container_management_service,
        object_management_service,
        std::move(garbage_collector),
        container_index,
        write_io_task_dispatcher,
        read_io_task_dispatcher,
        frontline_cache,
        object_io_executor,
        cache_accessor};

    //
    // Start the data store system. This will start the core
    // storage engine and the main server for handling data requests.
    //
    return lazarus_ds.start_data_store();
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