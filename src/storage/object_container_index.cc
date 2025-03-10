// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container_index.hh'
// Author: jcjuarez
// Description:
//      Indexes and routes object containers for all
//      data and metadata operations. 
// ****************************************************

#include <cassert>
#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "object_container_index.hh"

namespace lazarus
{
namespace storage
{

object_container_index::object_container_index(
    std::shared_ptr<storage_engine> storage_engine_handle)
    : storage_engine_{std::move(storage_engine_handle)},
      max_number_object_containers_{100u}
{}

object_container_index::~object_container_index()
{
    //
    // Close all the opened object container storage engine references.
    //
    tbb::concurrent_hash_map<std::string, object_container>::iterator it;

    for (it = object_container_index_table_.begin(); it != object_container_index_table_.end(); ++it)
    {
        rocksdb::ColumnFamilyHandle* storage_engine_reference = it->second.get_storage_engine_reference();

        if (storage_engine_reference)
        {
            storage_engine_->close_object_container_storage_engine_reference(
                storage_engine_reference);
        }
    }

    //
    // Cleanup the metadata table.
    //
    object_container_index_table_.clear();
}

void
object_container_index::insert_object_container(
    rocksdb::ColumnFamilyHandle* storage_engine_reference,
    const schemas::object_container_persistent_interface& object_container_persistent_metadata)
{
    //
    // At this point, it is guaranteed that the object container
    // reference has a valid hashable identifier to be used as index key.
    // Also, it is guaranteed that no other thread will try to insert the same key.
    //
    if (object_container_index_table_.insert({
        object_container_persistent_metadata.name(),
        object_container{
            storage_engine_reference,
            object_container_persistent_metadata}}))
    {
        //
        // Key did not exist and metadata register was succesful.
        // Exit execution.
        //
        return;
    }

    //
    // Reaching this point is a critical error as no other thread
    // should have reached this point before for inserting the same key.
    // Log the issue and assert.
    //
    spdlog::critical("Object container collision has been detected. "
        "ObjectContainerName={}.",
        object_container_persistent_metadata.name());

    assert(false);
}

rocksdb::ColumnFamilyHandle*
object_container_index::get_object_containers_internal_metadata_storage_engine_reference() const
{
    tbb::concurrent_hash_map<std::string, object_container>::const_accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_index::object_containers_internal_metadata_name))
    {
        return accessor->second.get_storage_engine_reference();
    }

    //
    // On failure, return a null reference.
    //
    return nullptr;
}

bool
object_container_index::object_container_exists(
    const char* object_container_name)
{
    tbb::concurrent_hash_map<std::string, object_container>::const_accessor accessor;
    return object_container_index_table_.find(accessor, object_container_name);
}

bool
object_container_index::is_internal_metadata(
    const std::string object_container_name)
{
    return object_container_name == rocksdb::kDefaultColumnFamilyName ||
        object_container_name == object_container_index::object_containers_internal_metadata_name;
}

} // namespace storage.
} // namespace lazarus.