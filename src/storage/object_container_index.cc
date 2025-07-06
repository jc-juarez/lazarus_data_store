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

using index_table_type = tbb::concurrent_hash_map<std::string, std::shared_ptr<object_container>>;

object_container_index::object_container_index(
    std::shared_ptr<storage_engine> storage_engine_handle)
    : storage_engine_{std::move(storage_engine_handle)}
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
    storage_engine_reference_handle* storage_engine_reference,
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

storage_engine_reference_handle*
object_container_index::get_object_containers_internal_metadata_storage_engine_reference() const
{
    const std::shared_ptr<object_container> object_container =
        get_object_container(object_containers_internal_metadata_name);

    //
    // The object containers internal metadata should always be valid.
    //
    assert(object_container != nullptr);

    return object_container->get_storage_engine_reference();
}

status::status_code
object_container_index::get_object_container_existence_status(
    const char* object_container_name) const
{
    index_table_type::const_accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_name))
    {
        //
        // Object container present in the index table.
        // If it is marked as deleted, consider it pending deletion.
        //
        return accessor->second->is_deleted() ?
            status::object_container_in_deletion_process : status::object_container_already_exists;
    }

    //
    // Not present in the index table; unknown object container.
    //
    return status::object_container_not_exists;
}

std::shared_ptr<object_container>
object_container_index::get_object_container(
    const char* object_container_name) const
{
    index_table_type::const_accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_name))
    {
        //
        // Object container present in the index table.
        //
        return accessor->second;
    }

    //
    // Not present in the index table; unknown object container.
    //
    return nullptr;
}

std::vector<std::shared_ptr<object_container>>
object_container_index::get_all_object_containers() const
{
    std::vector<std::shared_ptr<object_container>> object_containers;

    for (const auto& entry : object_container_index_table_)
    {
        object_containers.push_back(entry.second);
    }

    return object_containers;
}

status::status_code
object_container_index::remove_object_container(
    const char* object_container_name)
{
    index_table_type::accessor accessor;

    if (object_container_index_table_.find(
        accessor,
        object_container_name))
    {
        spdlog::info("Deleting object container reference from the index table. "
            "ObjectContainerMetadata={}.",
            accessor->second->to_string());

        object_container_index_table_.erase(accessor);

        return status::success;
    }

    return status::object_container_not_exists;
}

std::size_t
object_container_index::get_total_number_object_containers()
{
    return object_container_index_table_.size();
}

} // namespace storage.
} // namespace lazarus.