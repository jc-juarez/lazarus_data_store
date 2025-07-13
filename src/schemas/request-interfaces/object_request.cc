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
    : object_container_name_{},
      object_id_{},
      object_data_{},
      optype_{object_request_optype::invalid}
{
    const auto json = request->getJsonObject();

    if (json)
    {
        //
        // Parse the JSON into the system interface.
        //
        object_container_name_ = (*json)[object_container_name_key_tag].asString();
        object_id_ = (*json)[object_id_key_tag].asString();
        object_data_ = (*json)[object_data_key_tag].asString();
    }

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
    : object_container_name_{std::move(other.object_container_name_)},
      object_id_{std::move(other.object_id_)},
      object_data_{std::move(other.object_data_)},
      optype_{std::move(other.optype_)}
{}

const std::string&
object_request::get_object_container_name() const
{
    return object_container_name_;
}

const std::string&
object_request::get_object_id() const
{
    return object_id_;
}

const storage::byte_stream&
object_request::get_object_data() const
{
    return object_data_;
}

object_request_optype
object_request::get_optype() const
{
    return optype_;
}

} // namespace schemas.
} // namespace lazarus.