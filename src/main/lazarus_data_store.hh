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
class storage_engine;
class garbage_collector;
class object_container_index;
class write_io_dispatcher;
class object_management_service;
class object_container_management_service;
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
    // Entry point for the data store.
    //
    static
    exit_code
    run();

    //
    // Gets the stop source token for listening to termination requests.
    //
    static
    std::stop_token
    get_stop_source_token();

    //
    // Start the lazarus data store system.
    //
    status::status_code
    start_data_store() const;

private:

    //
    // Initializes the global logger to be used by the system.
    //
    static
    void
    initialize_logger(
        const boost::uuids::uuid session_id,
        const logger::logger_configuration logger_config);

    //
    // Registers the signals to listen to for termination.
    //
    static
    void
    register_signals();

    //
    // Signals termination requests from the OS through the stop source.
    //
    static
    void
    signal_handler(std::int32_t signal);

    //
    // Stop source for handling graceful terminations.
    //
    static std::stop_source stop_source_;

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
    std::shared_ptr<storage::storage_engine> storage_engine_;

    //
    // Object container management service handle.
    //
    std::shared_ptr<storage::object_container_management_service> object_container_management_service_;

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
    std::shared_ptr<storage::object_container_index> object_container_index_;

    //
    // Write request dispatcher handle.
    //
    std::shared_ptr<storage::write_io_dispatcher> write_request_dispatcher_;
};

} // namespace lazarus.
