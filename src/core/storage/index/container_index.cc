// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_index.hh'
// Author: jcjuarez
// Description:
//      Indexes and routes object containers for all
//      data and metadata operations. 
// ****************************************************

#include <cassert>
#include <spdlog/spdlog.h>
#include "container_index.hh"

namespace lazarus
{
namespace storage
{

container_index::container_index(
    const std::uint16_t number_container_buckets)
    : container_index_table_{number_container_buckets, container_bucket{}},
      number_container_buckets_{number_container_buckets},
      number_containers_{0u}
{
    for (std::uint16_t index = 0; index < number_container_buckets; ++index)
    {
        //
        // Assign the associated index to each bucket.
        //
        container_index_table_.at(index).set_index(index);
    }
}

bool
container_index::is_internal_metadata_container(
    const std::string& container_name)
{
    return container_name == rocksdb::kDefaultColumnFamilyName ||
           container_name == k_container_metadata_name;
}

status::status_code
container_index::insert_container(
    const schemas::container_persistent_interface& container_persistent_metadata,
    const std::vector<container_partition_metadata>& container_instances)
{
    const std::uint16_t bucket_index = get_associated_bucket_index(
        container_persistent_metadata.name());

    status::status_code status = container_index_table_.at(bucket_index).insert_container(
        container_persistent_metadata,
        container_instances);

    if (status::succeeded(status))
    {
        //
        // Insertion succeeded, increment the count of object containers in the system.
        //
        number_containers_.fetch_add(1u, std::memory_order_acq_rel);
    }

    return status;
}

storage_engine_reference_handle*
container_index::get_container_metadata_engine_reference() const
{
    const std::shared_ptr<container> container =
        get_container(k_container_metadata_name);

    //
    // The container metadata column family handle is a special case
    // because it is not associated with the structured data partitions.
    // Instead, it lives inside the free-ranging container metadata data partition,
    // which means there is only one instance for the container, so we need to
    // dereference the first element in its respective instances list.
    //
    // For this special case, passing collocation_index=0 does
    // not mean that it lives on partition 0th, but instead we are
    // just trying to get the first and only element in the instances list.
    //
    return container->get_engine_reference(
        0u /* collocation_index */);
}

status::status_code
container_index::get_container_existence_status(
    const std::string& container_name) const
{
    const std::shared_ptr<container> container =
        get_container(container_name);

    if (container == nullptr)
    {
        //
        // Not present in the index table; unknown object container.
        //
        return status::container_not_exists;
    }

    //
    // Object container present in the index table.
    // If it is marked as deleted, consider it pending deletion.
    //
    return container->is_deleted() ?
           status::container_in_deletion_process : status::container_already_exists;
}

std::shared_ptr<container>
container_index::get_container(
    const std::string& container_name) const
{
    const std::uint16_t bucket_index = get_associated_bucket_index(
        container_name);

    return container_index_table_.at(bucket_index).get_container(container_name);
}

std::vector<std::shared_ptr<container>>
container_index::get_all_containers_from_bucket(
    const std::uint16_t bucket_index) const
{
    return container_index_table_.at(bucket_index).get_all_containers();
}

status::status_code
container_index::remove_container(
    const std::string& container_name)
{
    const std::uint16_t bucket_index = get_associated_bucket_index(
        container_name);

    status::status_code status = container_index_table_.at(bucket_index).remove_container(
        container_name);

    if (status::succeeded(status))
    {
        //
        // Deletion succeeded, decrement the count of object containers in the system.
        //
        number_containers_.fetch_sub(1u, std::memory_order_acq_rel);
    }

    return status;
}

std::size_t
container_index::get_total_number_containers() const
{
    return number_containers_;
}

std::uint16_t
container_index::get_number_container_buckets() const
{
    return number_container_buckets_;
}

std::uint16_t
container_index::get_associated_bucket_index(
    const std::string& container_name) const
{
    return hasher_(container_name) % number_container_buckets_;
}

} // namespace storage.
} // namespace lazarus.