// ****************************************************
// Lazarus Data Store
// Main
// 'lazarus_data_store.cc'
// Author: jcjuarez
// Description:
//      Lazarus Data Store root object. 
// ****************************************************

#include <drogon/drogon.h>
#include "lazarus_data_store.hh"
#include "../network/server/server.hh"
#include "../network/server/server_configuration.hh"
#include "../network/endpoints/object_container_endpoint.hh"

namespace lazarus
{

lazarus_data_store::lazarus_data_store()
    : server_{std::make_shared<network::server>(lazarus::network::server_configuration{})}
{}

void
lazarus_data_store::start_system()
{
    //
    // Register the endpoints that the server needs to handle.
    //
    register_endpoints();

    //
    // As a final step, start the
    // HTTP server for handling incoming requests.
    //
    server_->start();
}

void
lazarus_data_store::register_endpoints()
{
    //
    // Object container endpoint.
    //
    server_->register_endpoint(std::make_shared<object_container_endpoint>("Hello"));
}

} // namespace lazarus.
