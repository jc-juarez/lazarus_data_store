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
#include "../status/status.hh"
#include "../common/uuid_utilities.hh"
#include "../logger/logger_configuration.hh"
#include "../network/server/server_configuration.hh"
#include "../storage/storage_engine_configuration.hh"

namespace lazarus
{

using exit_code = std::int32_t;

namespace network
{
class server;
}

namespace storage
{
class storage_engine;
class data_store_service;
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
        const storage::storage_engine_configuration& storage_engine_configuration);

    //
    // Entry point for the data store.
    //
    static
    exit_code
    run();

    //
    // Start the lazarus data store system.
    //
    status::status_code
    start_data_store();

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
    // Data store service handle.
    //
    std::shared_ptr<storage::data_store_service> data_store_service_;
};

} // namespace lazarus.
