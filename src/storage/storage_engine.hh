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
#include "../status/status.hh"
#include "storage_engine_configuration.hh"

namespace lazarus
{
namespace storage
{

using byte = char;
using byte_stream = std::string;

//
// Core storage engine with a RocksDB backend.
//
class storage_engine
{
public:

    //
    // Constructor for the storage engine.
    //
    storage_engine(
        const storage_engine_configuration& storage_engine_configuration);

    //
    // Starts the storage engine.
    // Requires a list of all object containers present on disk
    // and returns a list in respective order with all column family references.
    //
    status::status_code
    start(
        const std::vector<std::string>& object_containers_names,
        std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>* storage_engine_references_mapping);

    //
    // Inserts a single object into the data store.
    //
    status::status_code
    insert_object(
        rocksdb::ColumnFamilyHandle* object_container_storage_engine_reference,
        const char* object_id,
        const byte* object_data);

    //
    // Get an object from the data store.
    // Stores the object contents into the data stream if it exists.
    //
    status::status_code
    get_object(
        rocksdb::ColumnFamilyHandle* object_container_storage_engine_reference,
        const char* object_id,
        byte_stream& object_data);

    //
    // Creates a new object container inside the data store.
    // Returns the associated column family reference on success.
    //
    status::status_code
    create_object_container(
        const char* object_container_name,
        rocksdb::ColumnFamilyHandle* object_container_storage_engine_reference);

    //
    // Gets all the objects from a specified object container.
    // Returns them in an unordered fashion.
    //
    status::status_code
    get_all_objects_from_object_container(
        rocksdb::ColumnFamilyHandle* object_container_storage_engine_reference,
        std::unordered_map<std::string, byte_stream>* objects);

    //
    // Fetches the existing object containers in the data store.
    // In case of a system crash, the underlying core engine ensures
    // that all operations are written into the WAL entries, thus lazarus
    // only acknowledges object container references that were successfully
    // recorded into the storage engine WAL.
    // This should be invoked before starting the storage engine.
    //
    status::status_code
    fetch_object_containers_from_disk(
        std::vector<std::string>* object_containers_names);

private:

    //
    // Main handle for the underlying storage backend database.
    //
    std::unique_ptr<rocksdb::DB> core_database_;

    //
    // Configuration for the storage engine.
    //
    const storage_engine_configuration storage_engine_configuration_;
};

} // namespace storage.
} // namespace lazarus.