// ****************************************************
// Lazarus Data Store
// Schemas
// 'object_container_request_interface.cc'
// Author: jcjuarez
// Description:
//      Structure for defining an object container
//      from a network request. 
// ****************************************************

#include "object_container_request_interface.hh"

namespace lazarus
{
namespace schemas
{

object_container_request_interface::object_container_request_interface(
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
            optype_ = object_container_request_optype::create;
            break;
        }
        case drogon::HttpMethod::Delete:
        {
            optype_ = object_container_request_optype::remove;
            break;
        }
        default:
        {
            optype_ = object_container_request_optype::invalid;
            break;
        }
    }
}

object_container_request_interface::object_container_request_interface(
    object_container_request_interface&& other)
    : name_{std::move(other.name_)},
      optype_{std::move(other.optype_)}
{}

const std::string&
object_container_request_interface::get_name() const
{
    return name_;
}

object_container_request_optype
object_container_request_interface::get_optype() const
{
    return optype_;
}

} // namespace schemas.
} // namespace lazarus.