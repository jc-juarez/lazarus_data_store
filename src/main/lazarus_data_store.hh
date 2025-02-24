// ****************************************************
// Lazarus Data Store
// Main
// 'lazarus_data_store.hh'
// Author: jcjuarez
// Description:
//      Lazarus Data Store root object. 
// ****************************************************

#pragma once

#include <memory>
#include "../common/uuid_utilities.hh"
#include "../logger/logger_configuration.hh"
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
class data_store_accessor;
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
        const logger::logger_configuration& logger_config,
        const network::server_configuration& server_config);

    //
    // Start the lazarus data store system.
    //
    void
    start_system();

private:

    //
    // Initializes the global logger to be used by the system.
    //
    void
    initialize_logger();

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
    // Data store accessor handle.
    //
    std::shared_ptr<storage::data_store_accessor> data_store_accessor_;
};

} // namespace lazarus.
