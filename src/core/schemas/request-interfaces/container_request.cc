// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Schemas
// 'container_request.cc'
// Author: jcjuarez
// Description:
//      Structure for defining an object container
//      from a network request. 
// ****************************************************

#include "container_request.hh"

namespace lazarus
{
namespace schemas
{

container_request::container_request(
    const drogon::HttpRequestPtr& request)
    : name_{""}
{
    const auto json = request->getJsonObject();

    if (json)
    {
        //
        // Parse the JSON into the system interface.
        //
        name_ = (*json)[name_key_tag].asString();
    }

    switch (request->getMethod())
    {
        case drogon::HttpMethod::Put:
        {
            optype_ = container_request_optype::create;
            break;
        }
        case drogon::HttpMethod::Delete:
        {
            optype_ = container_request_optype::remove;
            break;
        }
        default:
        {
            optype_ = container_request_optype::invalid;
            break;
        }
    }
}

container_request::container_request(
    container_request&& other)
    : name_{std::move(other.name_)},
      optype_{std::move(other.optype_)}
{}

const std::string&
container_request::get_name() const
{
    return name_;
}

container_request_optype
container_request::get_optype() const
{
    return optype_;
}

} // namespace schemas.
} // namespace lazarus.