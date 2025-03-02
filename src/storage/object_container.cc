// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container.hh'
// Author: jcjuarez
// Description:
//      Object container metadata structure. Object
//      containers are logical buckets for holding
//      closely related data inside the data store. 
// ****************************************************

#include "object_container.hh"

namespace lazarus
{
namespace storage
{

object_container::object_container(
    const lazarus::schemas::object_container_persistance_interface& object_container_persistance)
    : id_{object_container_persistance.id()}
{}

} // namespace storage.
} // namespace lazarus.