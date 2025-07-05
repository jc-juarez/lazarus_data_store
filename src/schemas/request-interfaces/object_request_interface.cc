// ****************************************************
// Lazarus Data Store
// Schemas
// 'object_request_interface.cc'
// Author: jcjuarez
// Description:
//      Structure for defining an object
//      from a network request.
// ****************************************************

#include "object_request_interface.hh"

namespace lazarus
{
namespace schemas
{

object_request_interface::object_request_interface(
    const drogon::HttpRequestPtr& request)
    : object_container_name_{},
      object_id_{},
      object_data_{},
      optype_{object_request_optype::invalid}
{}

object_request_interface::object_request_interface(
    object_request_interface&& other)
    : object_container_name_{std::move(other.object_container_name_)},
      object_id_{std::move(other.object_id_)},
      object_data_{std::move(other.object_data_)},
      optype_{std::move(other.optype_)}
{}

const char*
object_request_interface::get_object_container_name() const
{
    return object_container_name_.c_str();
}

const char*
object_request_interface::get_object_id() const
{
    return object_id_.c_str();
}

const storage::byte*
object_request_interface::get_object_data() const
{
    return object_data_.data();
}

object_request_optype
object_request_interface::get_optype() const
{
    return optype_;
}

} // namespace schemas.
} // namespace lazarus.