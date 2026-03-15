// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Common
// 'args_validations.cc'
// Author: jcjuarez
// Description:
//      Basic validations for the process arguments.
// ****************************************************

#include <string>
#include <format>
#include <cstdint>
#include <filesystem>
#include "args_validations.hh"

namespace pandora
{
namespace common
{

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
        "Invalid number of process arguments provided for starting pandora. "
        "ArgsCount={}, "
        "MaxArgsCount={}.",
        args_count,
        max_args_count));
    }

    const std::string& config_file_path = args.back();

    if (config_file_path.empty())
    {
        throw std::runtime_error(
        "Provided configuration file path for starting pandora is empty");
    }

    if (!std::filesystem::exists(config_file_path))
    {
        throw std::runtime_error(std::format(
        "Provided configuration file path for starting pandora not found in the filesystem. "
        "ConfigFilePath={}.",
        config_file_path));
    }

    if (!std::filesystem::is_regular_file(config_file_path))
    {
        throw std::runtime_error(std::format(
        "Provided configuration file path for starting pandora is not a file. "
        "ConfigFilePath={}.",
        config_file_path));
    }
}

} // namespace common.
} // namespace pandora.