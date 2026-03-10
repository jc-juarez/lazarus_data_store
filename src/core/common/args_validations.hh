// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Common
// 'args_validations.hh'
// Author: jcjuarez
// Description:
//      Basic validations for the process arguments.
// ****************************************************

#pragma once

#include <vector>

namespace lazarus
{
namespace common
{

//
// The system only expects at most two arguments,
// the first being the path of the executable and the second the config file path.
//
inline constexpr std::uint8_t max_args_count = 2u;

//
// Validates the arguments provided by the process execution call.
//
void
validate_process_args(
    const std::vector<std::string>& args);

} // namespace common.
} // namespace lazarus.