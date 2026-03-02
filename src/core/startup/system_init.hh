// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'collocation_builder.hh'
// Author: jcjuarez
// Description:
//      Contains the system initialization logic.
// ****************************************************

#pragma once

#include <stop_token>
#include "../status/status.hh"
#include "../common/aliases.hh"
#include "../common/uuid_utilities.hh"
#include "../common/system_configuration.hh"
#include "../logger/logger_configuration.hh"

namespace lazarus
{

//
// Stop source for handling graceful terminations.
//
std::stop_source stop_source;

//
// Entry point for the data store.
//
exit_code
init_system(
    const std::vector<std::string>& args);

//
// Initializes all global dependencies.
//
void
init_global_dependencies(
    const boost::uuids::uuid session_id,
    const common::system_configuration& system_config);

//
// Initializes the global logger to be used by the system.
//
void
init_logger(
    const boost::uuids::uuid session_id,
    const logger::logger_configuration logger_config);

//
// Generates the system configs to be used.
//
common::system_configuration
generate_system_configs(
    const std::vector<std::string>& args);

//
// Gets the stop source token for listening to termination requests.
//
std::stop_token
get_stop_source_token();

//
// Registers the signals to listen to for termination.
//
void
register_signals();

//
// Signals termination requests from the OS through the stop source.
//
void
signal_handler(
    std::int32_t signal);

} // namespace lazarus.