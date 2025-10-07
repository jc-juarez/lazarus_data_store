// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Schemas
// 'object_request.cc'
// Author: jcjuarez
// Description:
//      Structure for defining an object
//      from a network request.
// ****************************************************

#include "object_request.hh"

namespace lazarus
{
namespace schemas
{

object_request::object_request(
    const drogon::HttpRequestPtr& request)
    : object_{nullptr},
      optype_{object_request_optype::invalid}
{
    const auto json = request->getJsonObject();

    std::string object_id{};
    storage::byte_stream object_data{};
    std::string container_name{};

    if (json)
    {
        //
        // Parse the JSON object and transfer
        // the object data into the system interface.
        //
        object_id = std::move((*json)[object_id_key_tag].asString());
        object_data = std::move((*json)[object_data_key_tag].asString());
        container_name = std::move((*json)[container_name_key_tag].asString());
    }

    //
    // Allocate the object with the fields from the request, if any.
    //
    object_ = std::make_unique<storage::object>(
        std::move(object_id),
        std::move(object_data),
        std::move(container_name));

    switch (request->getMethod())
    {
        case drogon::HttpMethod::Post:
        {
            optype_ = object_request_optype::insert;
            break;
        }
        case drogon::HttpMethod::Get:
        {
            optype_ = object_request_optype::get;
            break;
        }
        case drogon::HttpMethod::Delete:
        {
            optype_ = object_request_optype::remove;
            break;
        }
        default:
        {
            optype_ = object_request_optype::invalid;
            break;
        }
    }
}

object_request::object_request(
    object_request&& other)
    : object_{std::move(other.object_)},
      optype_{other.optype_}
{}

const std::string&
object_request::get_container_name() const
{
    return object_->get_container_name();
}

const std::string&
object_request::get_object_id() const
{
    return object_->get_object_id();
}

const storage::byte_stream&
object_request::get_object_data() const
{
    return object_->get_object_data();
}

std::string&
object_request::get_container_name_mutable()
{
    return object_->get_container_name();
}

std::string&
object_request::get_object_id_mutable()
{
    return object_->get_object_id();
}

storage::byte_stream&
object_request::get_object_data_mutable()
{
    return object_->get_object_data();
}

object_request_optype
object_request::get_optype() const
{
    return optype_;
}

} // namespace schemas.
} // namespace lazarus.