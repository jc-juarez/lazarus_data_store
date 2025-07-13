// ****************************************************
// Lazarus Data Store
// Storage
// 'container_index.hh'
// Author: jcjuarez
// Description:
//      Indexes and routes object containers for all
//      data and metadata operations. 
// ****************************************************

#pragma once

#include <vector>
#include <memory>
#include <tbb/tbb.h>
#include <functional>
#include "container_bucket.hh"
#include "container.hh"
#include "../status/status.hh"
#include "object_container_persistent_interface.pb.h"

namespace lazarus
{
namespace storage
{

class storage_engine;

//
// Core object container indexing system.
//
class container_index
{
public:

    //
    // Constructor for the object container index.
    //
    container_index(
        const std::uint16_t number_container_buckets,
        std::shared_ptr<storage_engine> storage_engine);

    //
    // Internal column family name for persisting
    // user-created object containers and their metadata.
    // This name is reserved for the data store use.
    //
    static constexpr const char* object_containers_internal_metadata_name =
        "_internal_metadata_:object_containers";

    //
    // Checks if an object container is part of the internal metadata.
    //
    static
    bool
    is_internal_metadata(
        const std::string object_container_name);

    //
    // Inserts a new object container entry into the index map.
    // This can either be invoked as a response_callback from a request-initiated
    // object container insertion or as the initial disk fetching process.
    // This is only executed after a well-known commited disk write.
    //
    status::status_code
    insert_object_container(
        storage_engine_reference_handle* storage_engine_reference,
        const schemas::object_container_persistent_interface& object_container_persistent_metadata);

    //
    // Gets the storage engine reference of the object containers internal metadata column family.
    //
    storage_engine_reference_handle*
    get_object_containers_internal_metadata_storage_engine_reference() const;

    //
    // Checks if the object container exists in the index internal metadata.
    // Returns the appropriate status code depending on the state of the object container.
    //
    status::status_code
    get_object_container_existence_status(
        const std::string& object_container_name) const;

    //
    // Gets a reference an object container.
    // Returns nullptr in case it does not exist.
    //
    std::shared_ptr<container>
    get_object_container(
        const std::string& object_container_name) const;

    //
    // Gets a list of all the object containers in a given bucket.
    //
    std::vector<std::shared_ptr<container>>
    get_all_object_containers_from_bucket(
        const std::uint16_t bucket_index) const;

    //
    // Removes an object container from the index table. After this, all subsequent
    // API calls to the object container index will not contain the object container.
    //
    status::status_code
    remove_object_container(
        const std::string& object_container_name);

    //
    // Gets the total current number of
    // object containers present in the system.
    // This API returns the total number of active and soft-deleted containers.
    //
    std::size_t
    get_total_number_object_containers() const;

    //
    // Gets the number of buckets in the index table.
    // Useful for executing index-based traversals.
    //
    std::uint16_t
    get_number_container_buckets() const;

private:

    //
    // Gets the container bucket index by hashing the given container name.
    //
    std::uint16_t
    get_associated_bucket_index(
        const std::string& container_name) const;

    //
    // Index table for object containers in the system.
    // Maps an object container identifier to the
    // respective object container memory reference.
    //
    std::vector<container_bucket> container_index_table_;

    //
    // Hasher for routing a container name to its respective bucket.
    //
    std::hash<std::string> hasher_;

    //
    // Number of container buckets in the index table.
    //
    const std::uint16_t number_container_buckets_;

    //
    // Keeps track of the total number of object containers in the system.
    // This is running-value as to avoid iterating over all buckets and asking for their sizes.
    //
    std::atomic<std::uint32_t> number_object_containers_;
};

} // namespace storage.
} // namespace lazarus.