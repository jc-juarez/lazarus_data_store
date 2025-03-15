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
    static constexpr const char* object_containers_internal_metadata_name = "_internal_metadata_:object_containers";

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
        rocksdb::ColumnFamilyHandle* storage_engine_reference,
        const schemas::object_container_persistent_interface& object_container_persistent_metadata);

    //
    // Gets the storage engine reference of the object containers internal metadata column family.
    //
    rocksdb::ColumnFamilyHandle*
    get_object_containers_internal_metadata_storage_engine_reference() const;

    //
    // Gets a snapshot of the current object containers internal metadata persistent metadata.
    // Used for modification of the metadata from a single writer thread (serializer).
    //
    std::optional<schemas::object_container_persistent_interface>
    get_object_container_persistent_metatadata_snapshot(
        const char* object_container_name) const;
        
    //
    // Sets the persistent metadata for a given object container.
    //
    status::status_code
    set_object_container_persistent_metadata(
        const char* object_container_name,
        const schemas::object_container_persistent_interface& object_container_persistent_metadata);

    //
    // Gets the storage engine reference for a particular object container.
    //
    rocksdb::ColumnFamilyHandle*
    get_object_container_storage_engine_reference(
        const char* object_container_name) const;

    //
    // Checks if the object container exists in the index internal metadata.
    //
    bool
    object_container_exists(
        const char* object_container_name);

    //
    // Gets the object container format as a string.
    //
    std::string
    get_object_container_as_string(
        const char* object_container_name);

private:

    //
    // Index table for object containers in the system.
    // Maps an object container identifier to the
    // respective object container memory reference.
    //
    tbb::concurrent_hash_map<std::string, std::unique_ptr<object_container>> object_container_index_table_;

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