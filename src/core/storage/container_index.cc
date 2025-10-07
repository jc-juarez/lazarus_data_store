// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
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
#include "storage_engine.hh"
#include "container_index.hh"
#include "../common/uuid_utilities.hh"

namespace lazarus
{
namespace storage
{

container_index::container_index(
    const std::uint16_t number_container_buckets,
    std::shared_ptr<storage_engine_interface> storage_engine)
    : container_index_table_{number_container_buckets, container_bucket{std::move(storage_engine)}},
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
        container_name == containers_internal_metadata_name;
}

status::status_code
container_index::insert_container(
    storage_engine_reference_handle* storage_engine_reference,
    const schemas::container_persistent_interface& container_persistent_metadata)
{
    const std::uint16_t bucket_index = get_associated_bucket_index(
        container_persistent_metadata.name());

    status::status_code status = container_index_table_.at(bucket_index).insert_container(
        storage_engine_reference,
        container_persistent_metadata);

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
container_index::get_containers_internal_metadata_storage_engine_reference() const
{
    const std::shared_ptr<container> container =
        get_container(containers_internal_metadata_name);

    return container->get_storage_engine_reference();
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