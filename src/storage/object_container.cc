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
    const schemas::object_container_persistent_interface& object_container_persistent_metadata)
    : storage_engine_reference_{storage_engine_reference},
      object_container_persistent_metadata_{object_container_persistent_metadata}
{}

rocksdb::ColumnFamilyHandle*
object_container::get_storage_engine_reference() const
{
    return storage_engine_reference_;
}

byte_stream
object_container::get_persistent_metadata_as_byte_stream() const
{
    byte_stream serialized_object_container_persistent_metadata;

    const bool is_serialization_successful = 
        object_container_persistent_metadata_.SerializeToString(&serialized_object_container_persistent_metadata);
    
    return is_serialization_successful ? serialized_object_container_persistent_metadata : byte_stream{};
}

bool
object_container::is_deleted() const
{
    return object_container_persistent_metadata_.is_deleted();
}

void
object_container::set_as_deleted()
{
    object_container_persistent_metadata_.set_is_deleted(true);
}

} // namespace storage.
} // namespace lazarus.