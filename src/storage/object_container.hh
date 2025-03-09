// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container.hh'
// Author: jcjuarez
// Description:
//      Object container metadata structure. Object
//      containers are logical buckets for holding
//      closely related data inside the data store. 
// ****************************************************

#pragma once

#include <string>
#include <rocksdb/db.h>
#include "object_container_persistent_interface.pb.h"

namespace lazarus
{
namespace storage
{

//
// Object container metadata internal structure.
// An in-memory creation of an object container reference can
// only be created after a well-known committed disk write its creation.
//
class object_container
{
public:

    //
    // Constructor for the object container.
    //
    object_container(
        rocksdb::ColumnFamilyHandle* storage_engine_reference,
        const schemas::object_container_persistent_interface& object_container_persistent_metadata);

    //
    // Gets the associated storage engine reference for the object container.
    //
    rocksdb::ColumnFamilyHandle*
    get_storage_engine_reference() const;

private:

    //
    // Object container persistent metadata.
    //
    schemas::object_container_persistent_interface object_container_persistent_metadata_;

    //
    // Pointer to the associated column family for the object container.
    // This only concerns the storage engine.
    //
    rocksdb::ColumnFamilyHandle* const storage_engine_reference_;
};

} // namespace storage.
} // namespace lazarus.