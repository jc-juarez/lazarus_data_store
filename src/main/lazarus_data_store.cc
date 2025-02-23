// ****************************************************
// Lazarus Data Store
// Main
// 'lazarus_data_store.cc'
// Author: jcjuarez
// Description:
//      Lazarus Data Store root object. 
// ****************************************************

#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <drogon/drogon.h>
#include "lazarus_data_store.hh"
#include "../network/server/server.hh"
#include <spdlog/sinks/basic_file_sink.h>
#include "../network/server/server_configuration.hh"

namespace lazarus
{

lazarus_data_store::lazarus_data_store()
    : server_{std::make_shared<network::server>(lazarus::network::server_configuration{})}
{}

void
lazarus_data_store::start_system()
{
    //
    // Initialize the logger to be used by the system.
    //
    initialize_logger();

    //
    // As a final step, start the
    // HTTP server for handling incoming requests.
    //
    server_->start();
}

void
lazarus_data_store::initialize_logger()
{
    spdlog::init_thread_pool(
        32768u, /* q_size */
        1u /* thread_count */);

    auto logger = spdlog::basic_logger_mt<spdlog::async_factory>("lazarus_ds_logger", "logs.txt");
    spdlog::set_default_logger(logger);
    spdlog::flush_every(std::chrono::seconds(2u));
}

} // namespace lazarus.
