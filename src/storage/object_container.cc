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
    rocksdb::ColumnFamilyHandle* storage_engine_reference,
    const lazarus::schemas::object_container_persistance_interface& object_container_persistent_metadata)
    : storage_engine_reference_{storage_engine_reference},
      object_container_persistent_metadata_{object_container_persistent_metadata}
{}

rocksdb::ColumnFamilyHandle*
object_container::get_storage_engine_reference() const
{
    return storage_engine_reference_;
}

} // namespace storage.
} // namespace lazarus.