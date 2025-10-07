// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Common
// 'response_utilities.hh'
// Author: jcjuarez
// Description:
//     Helper functions for server responses.
// ****************************************************

#pragma once

#include <string>
#include <cstdint>
#include "aliases.hh"
#include <unordered_map>

namespace lazarus::common::response_utilities
{

//
// Generates a server response in JSON
// format to be sent back to the client.
// Expects that the response fields given are references with a valid lifetime.
//
std::string
generate_server_json_response(
    const std::uint32_t internal_status_code,
    network::response_fields* response_fields);

} // namespace lazarus::common::response_utilities.