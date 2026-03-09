// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Main
// 'lazarus_data_store.hh'
// Author: jcjuarez
// Description:
//      Lazarus data store root object. 
// ****************************************************

#pragma once

#include <memory>
#include <expected>
#include <stop_token>
#include "../status/status.hh"
#include "../common/aliases.hh"
#include "../common/uuid_utilities.hh"
#include "../logger/logger_configuration.hh"
#include "../storage/gc/garbage_collector.hh"
#include "../storage/storage_configuration.hh"
#include "../network/server/server_configuration.hh"
#include "../storage/index/container_registry.hh"

namespace lazarus
{

namespace network
{
class server;
}

namespace storage
{
class data_partition;
class cache_accessor;
class frontline_cache;
class container_index;
class container_loader;
class read_io_executor;
class garbage_collector;
class collocation_resolver;
class io_dispatcher_interface;
class io_dispatcher_interface;
class data_partition_provider;
class storage_engine_interface;
class object_management_service;
class threading_context_provider;
class container_management_service;
}

//
// Lazarus system root object.
//
class lazarus_data_store
{
public:

    //
    // Constructor.
    //
    lazarus_data_store(
        const boost::uuids::uuid session_id,
        std::unique_ptr<storage::data_partition> container_metadata_partition,
        std::unique_ptr<storage::collocation_resolver> collocation_resolver,
        std::unique_ptr<storage::data_partition_provider> data_partition_provider,
        std::unique_ptr<storage::threading_context_provider> threading_context_provider,
        std::unique_ptr<network::server> server,
        std::unique_ptr<storage::container_management_service> container_management_service,
        std::unique_ptr<storage::object_management_service> object_management_service,
        std::unique_ptr<storage::garbage_collector> garbage_collector,
        std::unique_ptr<storage::container_index> container_index,
        std::unique_ptr<storage::io_dispatcher_interface> write_io_task_dispatcher,
        std::unique_ptr<storage::io_dispatcher_interface> read_io_task_dispatcher,
        std::unique_ptr<storage::frontline_cache> frontline_cache,
        std::unique_ptr<storage::read_io_executor> object_io_executor,
        std::unique_ptr<storage::cache_accessor> cache_accessor,
        std::unique_ptr<storage::container_loader> container_loader);

    //
    // Start the lazarus data store system.
    //
    status::status_code
    start_data_store();

private:

    //
    // Bootstraps the core storage state by booting the data partitions and
    // loading the container index state.
    //
    status::status_code
    bootstrap_storage_state();

    //
    // Initializes the structured data partitions and their respective storage engines.
    // Upon success, the complete list of all storage engine references in the system is returned back.
    //
    std::expected<
        storage::container_registry,
        status::status_code>
    boot_structured_data_partitions();

    //
    // Handles the boot process for a given data partition.
    // Upon success, the storage engine references for such engine is returned back.
    //
    status::status_code
    boot_data_partition(
        storage::data_partition& data_partition,
        std::unordered_map<std::string, storage::storage_engine_reference_handle*>& references_mapping);

    //
    // Session identifier.
    //
    boost::uuids::uuid session_id_;

    //
    // Containers metadata partition handle.
    //
    std::unique_ptr<storage::data_partition> container_metadata_partition_;

    //
    // Collocation resolver handle.
    //
    std::unique_ptr<storage::collocation_resolver> collocation_resolver_;

    //
    // Data partition provider handle.
    //
    std::unique_ptr<storage::data_partition_provider> data_partition_provider_;

    //
    // Threading context provider handle.
    //
    std::unique_ptr<storage::threading_context_provider> threading_context_provider_;

    //
    // HTTP server handle.
    //
    std::unique_ptr<network::server> server_;

    //
    // Object container management service handle.
    //
    std::unique_ptr<storage::container_management_service> container_management_service_;

    //
    // Object management service handle.
    //
    std::unique_ptr<storage::object_management_service> object_management_service_;

    //
    // Garbage collector handle.
    //
    std::unique_ptr<storage::garbage_collector> garbage_collector_;

    //
    // Object container index handle.
    //
    std::unique_ptr<storage::container_index> container_index_;

    //
    // Write request dispatcher handle.
    //
    std::unique_ptr<storage::io_dispatcher_interface> write_io_task_dispatcher_;

    //
    // Read request dispatcher handle.
    //
    std::unique_ptr<storage::io_dispatcher_interface> read_io_task_dispatcher_;

    //
    // Frontline cache handle.
    //
    std::unique_ptr<storage::frontline_cache> frontline_cache_;

    //
    // Object IO executor handle.
    //
    std::unique_ptr<storage::read_io_executor> object_io_executor_;

    //
    // Cache accessor handle.
    //
    std::unique_ptr<storage::cache_accessor> cache_accessor_;

    //
    // Container loader handle.
    //
    std::unique_ptr<storage::container_loader> container_loader_;
};

} // namespace lazarus.
