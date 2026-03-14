// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_bucket.cc'
// Author: jcjuarez
// Description:
//      Fundamental structure for storing containers.
// ****************************************************

#include <spdlog/spdlog.h>
#include "container_bucket.hh"

namespace lazarus::storage
{

container_bucket::container_bucket()
    : index_{0u}
{}

void
container_bucket::set_index(
    const std::uint16_t index)
{
    index_ = index;
}

status::status_code
container_bucket::insert_container(
    const schemas::container_persistent_interface& container_persistent_metadata,
    const std::vector<container_instance>& container_instances)
{
    //
    // At this point, it is guaranteed that the object container
    // reference has a valid hashable identifier to be used as index key.
    // Also, it is guaranteed that no other thread will try to insert the same key.
    //
    // Likewise, the container should hold a reference to the list of its respective storage
    // engines for closing its references upon destruction.
    //
    if (container_bucket_map_.exists(container_persistent_metadata.name()))
    {
        //
        // Reaching this point is a critical error as no other thread
        // should have reached this point before for inserting the same key.
        //
        return status::container_insertion_collision;
    }

    container_bucket_map_.insert(
        container_persistent_metadata.name(),
        std::make_shared<container>(
            container_persistent_metadata,
            container_instances));

    //
    // Key did not exist and metadata register was successful.
    // Exit execution.
    //
    spdlog::info("Inserted container to the bucket map. "
        "ContainerName={}, "
        "ContainerBucketIndex={}.",
        container_persistent_metadata.name().c_str(),
        index_);

    return status::success;
}

std::shared_ptr<container>
container_bucket::get_container(
    const std::string& container_name) const
{
    auto container = container_bucket_map_.get(container_name);
    return container.has_value() ? container.value() : nullptr;
}

std::vector<std::shared_ptr<container>>
container_bucket::get_all_containers() const
{
    std::vector<std::shared_ptr<container>> containers;
    auto bucket_entries = container_bucket_map_.get_all();

    for (auto& container_entry : bucket_entries)
    {
        containers.emplace_back(container_entry.second);
    }

    return containers;
}

status::status_code
container_bucket::remove_container(
    const std::string& container_name)
{
    if (!container_bucket_map_.exists(container_name))
    {
        return status::container_not_exists;
    }

    container_bucket_map_.erase(container_name);

    spdlog::info("Deleted object container reference from the bucket map. "
        "ContainerName={}, "
        "ContainerBucketIndex={}.",
        container_name.c_str(),
        index_);

    return status::success;
}

std::size_t
container_bucket::get_number_containers() const
{
    return container_bucket_map_.get_size();
}

} // namespace lazarus::storage.