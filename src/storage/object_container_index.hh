// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container_index.hh'
// Author: jcjuarez
// Description:
//      Indexes and routes object containers for all
//      data and metadata operations. 
// ****************************************************

#pragma once

#include <memory>
#include <tbb/tbb.h>
#include "object_container.hh"
#include "object_container_persistent_interface.pb.h"

namespace lazarus
{
namespace storage
{

class storage_engine;

//
// Core object container indexing system.
//
class object_container_index
{
public:

    //
    // Constructor for the object container index.
    //
    object_container_index(
        std::shared_ptr<storage_engine> storage_engine_handle);

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
    void
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
        const char* object_container_name) const;

    //
    // Gets a reference an object container.
    //
    std::shared_ptr<object_container>
    get_object_container(
        const char* object_container_name) const;

    //
    // Gets a list of all the object containers.
    //
    std::vector<std::shared_ptr<object_container>>
    get_all_object_containers() const;

    //
    // Removes an object container from the index table. After this, all subsequent
    // API calls to the object container index will not contain the object container.
    //
    status::status_code
    remove_object_container(
        const char* object_container_name);

private:

    //
    // Index table for object containers in the system.
    // Maps an object container identifier to the
    // respective object container memory reference.
    //
    tbb::concurrent_hash_map<std::string, std::shared_ptr<object_container>> object_container_index_table_;

    //
    // Max number of object containers allowed to be created
    // within the data store.
    //
    std::size_t max_number_object_containers_;

    //
    // Handle for the storage enine.
    //
    std::shared_ptr<storage_engine> storage_engine_;
};

} // namespace storage.
} // namespace lazarus.