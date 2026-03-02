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
    // Frontline cache component allocation.
    //
    frontline_cache_ = std::make_shared<storage::frontline_cache>(
    storage_configuration.number_frontline_cache_shards_,
    storage_configuration.max_frontline_cache_shard_size_mib_ * 1'024 * 1'024,
    storage_configuration.max_frontline_cache_shard_object_size_bytes,
    container_index_);

    //
    // Frontline cache accessor component allocation.
    //
    cache_accessor_ = std::make_shared<storage::cache_accessor>(
    frontline_cache_);

    //
    // Object IO executor component allocation.
    //
    object_io_executor_ = std::make_shared<storage::read_io_executor>(
    storage_engine_);

    //
    // Write batch aggregator component allocation.
    //
    auto write_batch_aggregator = std::make_unique<storage::write_batch_aggregator>(
    object_io_executor_,
    cache_accessor_);

    //
    // Write request dispatcher component allocation.
    //
    write_io_task_dispatcher_ = std::make_shared<storage::write_io_dispatcher>(
    std::move(write_batch_aggregator));

    //
    // Read request dispatcher component allocation.
    //
    read_io_task_dispatcher_ = std::make_shared<storage::read_io_dispatcher>(
    storage_configuration.number_read_io_threads_,
    object_io_executor_,
    cache_accessor_);

    //
    // Object management service component allocation.
    //
    object_management_service_ = std::make_shared<storage::object_management_service>(
    storage_configuration,
    container_index_,
    write_io_task_dispatcher_,
    read_io_task_dispatcher_,
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

} // namespace lazarus.