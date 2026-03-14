// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Logger
// 'logging.hh'
// Author: jcjuarez
// Description:
//      Logging utilities for the system.
// ****************************************************

#pragma once

#include <string>
#include <spdlog/spdlog.h>

namespace lazarus
{
namespace logger
{

struct context
{
    static thread_local std::string request_id;
};

} // namespace logger.
} // namespace lazarus.

//
// Global scope access.
//
#define TRACE_LOG(level, fmt, ...)                                   \
do                                                                   \
{                                                                    \
    const auto& ctx = lazarus::logger::context::request_id;          \
    if (ctx.empty())                                                 \
    {                                                                \
        spdlog::level(fmt, ##__VA_ARGS__);                           \
    }                                                                \
    else                                                             \
    {                                                                \
        spdlog::level("[{}] " fmt, ctx, ##__VA_ARGS__);              \
    }                                                                \
} while (0)
