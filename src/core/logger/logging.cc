// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Logger
// 'logging.cc'
// Author: jcjuarez
// Description:
//      Logging utilities for the system.
// ****************************************************

#include "logging.hh"

namespace lazarus
{
namespace logger
{

thread_local std::string context::request_id;

} // namespace logger.
} // namespace lazarus.