// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Common
// 'response_utilities.cc'
// Author: jcjuarez
// Description:
//     Helper functions for server responses.
// ****************************************************

#include <format>
#include "nlohmann/json.hpp"
#include "response_utilities.hh"

namespace lazarus::common::response_utilities
{

std::string
generate_server_json_response(
    const std::uint32_t internal_status_code,
    network::response_fields* response_fields)
{
    nlohmann::json response_body;
    response_body["internal_status_code"] = std::format("{:#x}", internal_status_code);

    if (response_fields != nullptr)
    {
        for (auto& [key, value] : *response_fields)
        {
            response_body[key] = *value;
        }
    }

    return response_body.dump();
}

} // namespace lazarus::common::response_utilities.