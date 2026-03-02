// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
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
#include <stop_token>
#include "../status/status.hh"
#include "../common/aliases.hh"
#include "../common/uuid_utilities.hh"
#include "../logger/logger_configuration.hh"
#include "../storage/gc/garbage_collector.hh"
#include "../storage/storage_configuration.hh"
#include "../network/server/server_configuration.hh"

namespace lazarus
{

namespace network
{
class server;
}

namespace storage
{
class garbage_collector;
class container_index;
class frontline_cache;
class io_dispatcher_interface;
class io_dispatcher_interface;
class storage_engine_interface;
class object_management_service;
class container_management_service;
class read_io_executor;
class cache_accessor;
class collocation_resolver;
class data_partition_provider;
class threading_context;
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
        const network::server_configuration& server_config,
        const storage::storage_configuration& storage_configuration);

    //
    // Start the lazarus data store system.
    //
    status::status_code
    start_data_store() const;

private:

    //
    // Session identifier.
    //
    boost::uuids::uuid session_id_;

    //
    // Logger configurations.
    //
    const logger::logger_configuration logger_config_;

    //
    // HTTP server handle.
    //
    std::shared_ptr<network::server> server_;

    //
    // Storage engine handle.
    //
    std::shared_ptr<storage::storage_engine_interface> storage_engine_;

    //
    // Object container management service handle.
    //
    std::shared_ptr<storage::container_management_service> container_management_service_;

    //
    // Object management service handle.
    //
    std::shared_ptr<storage::object_management_service> object_management_service_;

    //
    // Garbage collector handle.
    //
    std::unique_ptr<storage::garbage_collector> garbage_collector_;

    //
    // Object container index handle.
    //
    std::shared_ptr<storage::container_index> container_index_;

    //
    // Write request dispatcher handle.
    //
    std::shared_ptr<storage::io_dispatcher_interface> write_io_task_dispatcher_;

    //
    // Read request dispatcher handle.
    //
    std::shared_ptr<storage::io_dispatcher_interface> read_io_task_dispatcher_;

    //
    // Frontline cache handle.
    //
    std::shared_ptr<storage::frontline_cache> frontline_cache_;

    //
    // Object IO executor handle.
    //
    std::shared_ptr<storage::read_io_executor> object_io_executor_;

    //
    // Cache accessor handle.
    //
    std::shared_ptr<storage::cache_accessor> cache_accessor_;

    //
    // Collocation resolver handle.
    //
    std::shared_ptr<storage::collocation_resolver> collocation_resolver_;

    //
    // Data partition provider handle.
    //
    std::shared_ptr<storage::data_partition_provider> data_partition_provider_;

    //
    // Threading context provider handle.
    //
    std::shared_ptr<storage::threading_context> threading_context_provider_;
};

} // namespace lazarus.
