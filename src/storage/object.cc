// ****************************************************
// Lazarus Data Store
// Storage
// 'object.cc'
// Author: jcjuarez
// Description:
//      Wrapper for holding an object associated data.
// ****************************************************

#include "object.hh"

namespace lazarus::storage
{

object::object(
    std::string&& object_id,
    byte_stream&& object_data,
    std::string&& container_name)
    : object_id_{std::move(object_id)},
      object_data_{std::move(object_data)},
      container_name_{std::move(container_name)}
{}

object::object(
    object&& other)
    : object_id_{std::move(other.object_id_)},
      object_data_{std::move(other.object_data_)},
      container_name_{std::move(other.container_name_)}
{}

const std::string&
object::get_object_id() const
{
    return object_id_;
}

const byte_stream&
object::get_object_data() const
{
    return object_data_;
}

const std::string&
object::get_container_name() const
{
    return container_name_;
}

} // namespace lazarus::storage.