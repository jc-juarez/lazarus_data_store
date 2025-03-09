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

//
// Core object container indexing system.
//
class object_container_index
{
public:

    //
    // Constructor for the object container index.
    //
    object_container_index();

    //
    // Inserts a new object container entry into the index map.
    // This can either be invoked as a callback from a request-initiated
    // object container insertion or as the initial disk fetching process.
    // This is only executed after a well-known commited disk write.
    //
    void
    insert_object_container(
        rocksdb::ColumnFamilyHandle* storage_engine_reference,
        const schemas::object_container_persistent_interface& object_container_persistent_metadata);

    //
    // Internal column family name for persisting
    // user-created object containers and their metadata.
    // This name is reserved for the data store use.
    //
    static constexpr const char* object_containers_internal_metadata_column_family_name = "_internal_metadata_:object_containers";

    //
    // Sets the reference for the object containers internal metadata column family.
    //
    void
    set_object_containers_internal_metadata_handle(
        rocksdb::ColumnFamilyHandle* storage_engine_reference,
        const schemas::object_container_persistent_interface& object_container_persistent_metadata);

    //
    // Gets the data store reference of the object containers internal metadata column family.
    //
    rocksdb::ColumnFamilyHandle*
    get_object_containers_internal_metadata_storage_engine_reference() const;

    //
    // Checks if the object container exists in the index internal metadata.
    //
    bool
    object_container_exists(
        const char* object_container_name);

private:

    //
    // Main index for object containers in the system.
    // Maps an object container identifier to the
    // respective object container memory reference.
    //
    tbb::concurrent_hash_map<std::string, object_container> object_container_index_table_;

    //
    // Max number of object containers allowed to be created
    // within the data store.
    //
    std::size_t max_number_object_containers_;

    //
    // Main handle to the column family for storing user-created object
    // containers. Internally, it is treated as a normal object container as well.
    //
    std::unique_ptr<object_container> object_containers_internal_metadata_;
};

} // namespace storage.
} // namespace lazarus.