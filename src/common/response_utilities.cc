// ****************************************************
// Lazarus Data Store
// Schemas
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
    std::unordered_map<std::string, std::string>* additional_parameters)
{
    nlohmann::json response_body;
    response_body["internal_status_code"] = std::format("{:#x}", internal_status_code);

    if (additional_parameters != nullptr)
    {
        for (auto&& [key, value] : std::move(*additional_parameters))
        {
            response_body[key] = std::move(value);
        }
    }

    return response_body.dump();
}

} // namespace lazarus::common::response_utilities.