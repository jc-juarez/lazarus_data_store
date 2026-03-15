// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Logger
// 'logging.hh'
// Author: jcjuarez
// Description:
//      Logging utilities for the system.
// ****************************************************

#pragma once

#include <string>
#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include "logger_configuration.hh"
#include "../common/uuid_utilities.hh"
#include <spdlog/sinks/rotating_file_sink.h>

namespace pandora
{
namespace logger
{

//
// Initializes the global logger to be used by the system.
//
void
init_logger(
    const boost::uuids::uuid session_id,
    const logger_configuration& logger_config);

struct context
{
    static thread_local std::string request_id;
};

} // namespace logger.
} // namespace pandora.

//
// Global scope access.
//
#define TRACE_LOG(level, fmt, ...)                                   \
do                                                                   \
{                                                                    \
    const auto& ctx = pandora::logger::context::request_id;          \
    if (ctx.empty())                                                 \
    {                                                                \
        spdlog::level(fmt, ##__VA_ARGS__);                           \
    }                                                                \
    else                                                             \
    {                                                                \
        spdlog::level("[{}] " fmt, ctx, ##__VA_ARGS__);              \
    }                                                                \
} while (0)
