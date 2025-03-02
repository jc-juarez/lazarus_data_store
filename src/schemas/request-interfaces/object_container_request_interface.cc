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
    : id_{""}
{
    const auto json = request->getJsonObject();

    if (json)
    {
        //
        // Parse the JSON into the system interface.
        //
        id_ = (*json)[id_key_name].asString();
    }
}

object_container_request_interface::object_container_request_interface(
    object_container_request_interface&& other)
    : id_{std::move(other.id_)}
{}

const char*
object_container_request_interface::get_id() const
{
    return id_.c_str();
}

} // namespace schemas.
} // namespace lazarus.