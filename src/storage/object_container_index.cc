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
    const std::uint16_t number_container_buckets,
    std::shared_ptr<storage_engine> storage_engine)
    : container_index_table_{number_container_buckets, container_bucket{std::move(storage_engine)}},
      number_container_buckets_{number_container_buckets},
      number_object_containers_{0u}
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
object_container_index::is_internal_metadata(
    const std::string object_container_name)
{
    return object_container_name == rocksdb::kDefaultColumnFamilyName ||
        object_container_name == object_containers_internal_metadata_name;
}

status::status_code
object_container_index::insert_object_container(
    storage_engine_reference_handle* storage_engine_reference,
    const schemas::object_container_persistent_interface& object_container_persistent_metadata)
{
    const std::uint16_t bucket_index = get_associated_bucket_index(
        object_container_persistent_metadata.name());

    status::status_code status = container_index_table_.at(bucket_index).insert_container(
        storage_engine_reference,
        object_container_persistent_metadata);

    if (status::succeeded(status))
    {
        //
        // Insertion succeeded, increment the count of object containers in the system.
        //
        number_object_containers_.fetch_add(1u, std::memory_order_acq_rel);
    }

    return status;
}

storage_engine_reference_handle*
object_container_index::get_object_containers_internal_metadata_storage_engine_reference() const
{
    const std::shared_ptr<container> object_container =
        get_object_container(object_containers_internal_metadata_name);

    return object_container->get_storage_engine_reference();
}

status::status_code
object_container_index::get_object_container_existence_status(
    const std::string& object_container_name) const
{
    const std::shared_ptr<container> object_container =
        get_object_container(object_container_name);

    if (object_container == nullptr)
    {
        //
        // Not present in the index table; unknown object container.
        //
        return status::object_container_not_exists;
    }

    //
    // Object container present in the index table.
    // If it is marked as deleted, consider it pending deletion.
    //
    return object_container->is_deleted() ?
           status::object_container_in_deletion_process : status::object_container_already_exists;
}

std::shared_ptr<container>
object_container_index::get_object_container(
    const std::string& object_container_name) const
{
    const std::uint16_t bucket_index = get_associated_bucket_index(
        object_container_name);

    return container_index_table_.at(bucket_index).get_object_container(object_container_name);
}

std::vector<std::shared_ptr<container>>
object_container_index::get_all_object_containers_from_bucket(
    const std::uint16_t bucket_index) const
{
    return container_index_table_.at(bucket_index).get_all_object_containers();
}

status::status_code
object_container_index::remove_object_container(
    const std::string& object_container_name)
{
    const std::uint16_t bucket_index = get_associated_bucket_index(
        object_container_name);

    status::status_code status = container_index_table_.at(bucket_index).remove_object_container(
        object_container_name);

    if (status::succeeded(status))
    {
        //
        // Deletion succeeded, decrement the count of object containers in the system.
        //
        number_object_containers_.fetch_sub(1u, std::memory_order_acq_rel);
    }

    return status;
}

std::size_t
object_container_index::get_total_number_object_containers() const
{
    return number_object_containers_;
}

std::uint16_t
object_container_index::get_number_container_buckets() const
{
    return number_container_buckets_;
}

std::uint16_t
object_container_index::get_associated_bucket_index(
    const std::string& container_name) const
{
    return hasher_(container_name) % number_container_buckets_;
}

} // namespace storage.
} // namespace lazarus.