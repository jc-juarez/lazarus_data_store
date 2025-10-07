// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
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

#include <string>
#include <vector>
#include <format>
#include <cstdint>
#include <filesystem>

namespace lazarus
{
namespace common
{

//
// The system only expects at most two arguments,
// the first being the path of the executable and the second the config file path.
//
constexpr std::uint8_t max_args_count = 2u;

void
validate_process_args(
    const std::vector<std::string>& args)
{
    const std::size_t args_count = args.size();

    if (args_count == 1u)
    {
        // Nothing to validate.
        return;
    }

    if (args_count != max_args_count)
    {
        throw std::runtime_error(std::format(
            "Invalid number of process arguments provided for starting lazarus. "
            "ArgsCount={}, "
            "MaxArgsCount={}.",
            args_count,
            max_args_count));
    }

    const std::string& config_file_path = args.back();

    if (config_file_path.empty())
    {
        throw std::runtime_error(
            "Provided configuration file path for starting lazarus is empty");
    }

    if (!std::filesystem::exists(config_file_path))
    {
        throw std::runtime_error(std::format(
            "Provided configuration file path for starting lazarus not found in the filesystem. "
            "ConfigFilePath={}.",
            config_file_path));
    }

    if (!std::filesystem::is_regular_file(config_file_path))
    {
        throw std::runtime_error(std::format(
            "Provided configuration file path for starting lazarus is not a file. "
            "ConfigFilePath={}.",
            config_file_path));
    }
}

} // namespace common.
} // namespace lazarus.