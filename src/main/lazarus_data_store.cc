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
#include "../storage/storage_engine.hh"
#include "../storage/data_store_accessor.hh"
#include <spdlog/sinks/rotating_file_sink.h>
#include "../network/server/server_configuration.hh"

namespace lazarus
{

lazarus_data_store::lazarus_data_store(
    const logger::logger_configuration& logger_config,
    const network::server_configuration& server_config)
    : session_id_{common::generate_uuid()},
      logger_config_{logger_config}
{
    //
    // Storage engine component allocation.
    //
    storage_engine_ = std::make_shared<storage::storage_engine>();

    //
    // Data store accessor component allocation.
    //
    data_store_accessor_ = std::make_shared<storage::data_store_accessor>(
        storage_engine_);

    //
    // Server component allocation.
    //
    server_ = std::make_shared<network::server>(
        server_config,
        data_store_accessor_);
}

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
        logger_config_.queue_size_bytes_,
        1u /* thread_count */);

    const std::string current_session_logs_directory = logger_config_.logging_session_directory_prefix_ + "-" + common::uuid_to_string(session_id_);
    const std::filesystem::path logging_session_directory_path =
        logger_config_.logs_directory_path_ / current_session_logs_directory / logger_config_.log_file_prefix_;

    auto logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
        logger_config_.component_name_,
        logging_session_directory_path.string(),
        logger_config_.max_log_file_size_bytes_,
        logger_config_.max_number_files_for_session_);

    spdlog::set_default_logger(logger);
    spdlog::flush_every(std::chrono::milliseconds(logger_config_.flush_frequency_ms_));
}

} // namespace lazarus.
