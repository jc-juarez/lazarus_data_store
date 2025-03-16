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
#include "../common/uuid_utilities.hh"

namespace lazarus
{
namespace storage
{

object_container_index::object_container_index(
    std::shared_ptr<storage_engine> storage_engine_handle)
    : storage_engine_{std::move(storage_engine_handle)},
      max_number_object_containers_{100u}
{}

bool
object_container_index::is_internal_metadata(
    const std::string object_container_name)
{
    return object_container_name == rocksdb::kDefaultColumnFamilyName ||
        object_container_name == object_containers_internal_metadata_name;
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
    if (object_container_index_table_.emplace(
        object_container_persistent_metadata.name(),
        std::make_unique<object_container>(
            storage_engine_,
            storage_engine_reference,
            object_container_persistent_metadata)))
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
    return get_object_container_storage_engine_reference(
        object_containers_internal_metadata_name);
}

status::status_code
object_container_index::mark_object_container_as_deleted(
    const char* object_container_name)
{
    tbb::concurrent_hash_map<std::string, std::unique_ptr<object_container>>::accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_name))
    {
        accessor->second->mark_as_deleted();
        return status::success;
    }

    return status::object_container_not_exists;
}

rocksdb::ColumnFamilyHandle*
object_container_index::get_object_container_storage_engine_reference(
    const char* object_container_name) const
{
    tbb::concurrent_hash_map<std::string, std::unique_ptr<object_container>>::const_accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_name))
    {
        return accessor->second->get_storage_engine_reference();
    }

    //
    // On failure, return a null reference.
    //
    return nullptr;
}

status::status_code
object_container_index::object_container_exists(
    const char* object_container_name)
{
    tbb::concurrent_hash_map<std::string, std::unique_ptr<object_container>>::const_accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_name))
    {
        //
        // Object container present in the index table.
        // If it is marked as deleted, consider it pending deletion.
        //
        return accessor->second->is_deleted() ?
            status::object_container_in_deletion_process : status::success;
    }

    //
    // Not present in the index table; unknown object container.
    //
    return status::object_container_not_exists;
}

std::string
object_container_index::get_object_container_as_string(
    const char* object_container_name)
{
    tbb::concurrent_hash_map<std::string, std::unique_ptr<object_container>>::const_accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_name))
    {
        return accessor->second->to_string();
    }

    //
    // Not present in the index table; return an empty string.
    //
    return "";
}

std::optional<schemas::object_container_persistent_interface>
object_container_index::get_object_container_persistent_metatadata_snapshot(
    const char* object_container_name) const
{
    tbb::concurrent_hash_map<std::string, std::unique_ptr<object_container>>::const_accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_name))
    {
        return accessor->second->get_persistent_metadata_snapshot();
    }

    //
    // On failure, return a null optional.
    //
    return std::nullopt;
}

status::status_code
object_container_index::set_object_container_persistent_metadata(
    const char* object_container_name,
    const schemas::object_container_persistent_interface& object_container_persistent_metadata)
{
    tbb::concurrent_hash_map<std::string, std::unique_ptr<object_container>>::accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_name))
    {
        accessor->second->set_persistent_metadata(
            object_container_persistent_metadata);

        return status::success;
    }

    return status::object_container_not_exists;
}

} // namespace storage.
} // namespace lazarus.