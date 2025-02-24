// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_engine.hh'
// Author: jcjuarez
// Description:
//      Core storage engine for handling IO operations. 
// ****************************************************

#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <rocksdb/db.h>
#include "i_storage_operations.hh"

namespace lazarus
{
namespace storage
{

//
// Core storage engine with a RocksDB backend.
//
class storage_engine : public i_storage_operations
{
public:

    //
    // Constructor for the storage engine.
    //
    storage_engine();

    //
    // Inserts a single object into the data store.
    //
    void
    insert_object(
        const char* object_id,
        const byte* object_data_buffer);

    //
    // Get an object from the data store.
    // Stores the object contents into the data stream if it exists.
    //
    void
    get_object(
        const char* object_id,
        byte_stream& object_data_stream);

private:

    //
    // Main handle for the underlying storage backend database.
    //
    std::unique_ptr<rocksdb::DB> core_database_;
};

} // namespace storage.
} // namespace lazarus.