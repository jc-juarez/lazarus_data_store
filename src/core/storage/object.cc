// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
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

std::string&
object::get_object_id()
{
    return object_id_;
}

byte_stream&
object::get_object_data()
{
    return object_data_;
}

std::string&
object::get_container_name()
{
    return container_name_;
}

} // namespace lazarus::storage.