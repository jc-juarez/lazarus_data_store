// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_engine.hh'
// Author: jcjuarez
// Description:
//      Core storage engine for handling IO operations
//      with a RocksDB key-value store backend.
// ****************************************************

#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <rocksdb/db.h>
#include "../../status/status.hh"
#include "../../common/aliases.hh"
#include "../storage_configuration.hh"
#include "storage_engine_interface.hh"

namespace lazarus
{
namespace storage
{

class storage_engine : public storage_engine_interface
{
public:

    //
    // Constructor for the storage engine.
    //
    storage_engine();

    //
    // Sets the underlying persistent store backend to be used.
    // For now, only the RocksDB engine is supported.
    //
    void
    set_persistent_store(
        const std::uint16_t collocation_index,
        std::unique_ptr<rocksdb::DB> persistent_store) override;

    //
    // Inserts a single object into the data store.
    //
    status::status_code
    insert_object(
        storage_engine_reference_handle* container_storage_engine_reference,
        const char* object_id,
        const byte_stream& object_data) override;

    //
    // Get an object from the data store.
    // Stores the object contents into the data stream if it exists.
    //
    status::status_code
    get_object(
        storage_engine_reference_handle* container_storage_engine_reference,
        const char* object_id,
        byte_stream* object_data) override;

    //
    // Creates a new object container inside the data store.
    // Returns the associated column family reference on success.
    //
    status::status_code
    create_container(
        const char* container_name,
        storage_engine_reference_handle** container_storage_engine_reference) override;

    //
    // Gets all the objects from a specified object container.
    // Returns them in an unordered fashion.
    //
    status::status_code
    get_all_objects_from_container(
        storage_engine_reference_handle* container_storage_engine_reference,
        std::unordered_map<std::string, byte_stream>* objects) override;

    //
    // Closes the in-memory object container storage engine reference.
    //
    status::status_code
    close_container_storage_engine_reference(
        storage_engine_reference_handle* container_storage_engine_reference) override;

    //
    // Removes an object from a given object container.
    //
    status::status_code
    remove_object(
        storage_engine_reference_handle* container_storage_engine_reference,
        const char* object_id) override;

    //
    // Removes an object container permanently from the filesystem.
    //
    status::status_code
    remove_container(
        storage_engine_reference_handle* container_storage_engine_reference) override;

    //
    // Executes a batch of write operations (insert/remove) for objects.
    //
    status::status_code
    execute_objects_write_batch(
        storage_engine_write_batch& write_batch) override;

private:

    //
    // Main handle for the underlying storage backend key-value persistent store.
    //
    std::unique_ptr<rocksdb::DB> persistent_store_;

    //
    // Corresponding collocation index.
    //
    std::uint16_t collocation_index_;
};

} // namespace storage.
} // namespace lazarus.