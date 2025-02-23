// ****************************************************
// Lazarus Data Store
// Main
// 'lazarus_data_store.cc'
// Author: jcjuarez
// Description:
//      Lazarus Data Store root object. 
// ****************************************************

#include "lazarus_data_store.hh"
#include "../network/server/server.hh"
#include "../network/server/server_configuration.hh"

namespace lazarus
{

lazarus_data_store::lazarus_data_store()
    : server_{std::make_shared<server>(lazarus::server_configuration{})}
{}

void
lazarus_data_store::start_system()
{
    server_->start();
}

} // namespace lazarus.
