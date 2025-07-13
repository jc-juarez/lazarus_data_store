// ****************************************************
// Lazarus Data Store
// Storage
// 'container_bucket.cc'
// Author: jcjuarez
// Description:
//      Fundamental structure for storing containers.
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "container_bucket.hh"

namespace lazarus::storage
{

using index_table_type = tbb::concurrent_hash_map<std::string, std::shared_ptr<object_container>>;

container_bucket::container_bucket(
    std::shared_ptr<storage_engine> storage_engine)
    : storage_engine_{std::move(storage_engine)}
{}

void
container_bucket::set_index(
    const std::uint16_t index)
{
    index_ = index;
}

status::status_code
container_bucket::insert_container(
    storage_engine_reference_handle* storage_engine_reference,
    const schemas::object_container_persistent_interface& object_container_persistent_metadata)
{
    //
    // At this point, it is guaranteed that the object container
    // reference has a valid hashable identifier to be used as index key.
    // Also, it is guaranteed that no other thread will try to insert the same key.
    //
    if (container_bucket_map_.emplace(
        object_container_persistent_metadata.name(),
        std::make_unique<object_container>(
            storage_engine_,
            storage_engine_reference,
            object_container_persistent_metadata)))
    {
        //
        // Key did not exist and metadata register was successful.
        // Exit execution.
        //
        spdlog::info("Inserted container to the bucket map. "
            "ObjectContainerName={}, "
            "ContainerBucketIndex={}.",
            object_container_persistent_metadata.name().c_str(),
            index_);

        return status::success;
    }

    //
    // Reaching this point is a critical error as no other thread
    // should have reached this point before for inserting the same key.
    //
    return status::container_insertion_collision;
}

std::shared_ptr<object_container>
container_bucket::get_object_container(
    const std::string& object_container_name) const
{
    index_table_type::const_accessor accessor;

    if (container_bucket_map_.find(
        accessor,
        object_container_name))
    {
        //
        // Object container present in the bucket map.
        //
        return accessor->second;
    }

    //
    // Not present in the bucket map; unknown object container.
    //
    return nullptr;
}

std::vector<std::shared_ptr<object_container>>
container_bucket::get_all_object_containers() const
{
    std::vector<std::shared_ptr<object_container>> object_containers;

    for (const auto& entry : container_bucket_map_)
    {
        object_containers.push_back(entry.second);
    }

    return object_containers;
}

status::status_code
container_bucket::remove_object_container(
    const std::string& object_container_name)
{
    index_table_type::accessor accessor;

    if (container_bucket_map_.find(
        accessor,
        object_container_name))
    {
        container_bucket_map_.erase(accessor);

        spdlog::info("Deleted object container reference from the bucket map. "
            "ObjectContainerMetadata={}, "
            "ContainerBucketIndex={}.",
            accessor->second->to_string(),
            index_);

        return status::success;
    }

    return status::object_container_not_exists;
}

std::size_t
container_bucket::get_number_object_containers() const
{
    return container_bucket_map_.size();
}

} // namespace lazarus::storage.