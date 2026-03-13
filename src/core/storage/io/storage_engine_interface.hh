// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_engine.hh'
// Author: jcjuarez
// Description:
//      Interface for the core storage engine
//      for handling IO operations.
// ****************************************************

#pragma once

#include <string>
#include "../../status/status.hh"
#include "../../common/aliases.hh"
#include "../../common/interface.hh"
#include "../storage_configuration.hh"

namespace lazarus
{
namespace storage
{

class storage_engine_interface : public common::interface
{
public:

    //
    // Sets the underlying persistent store backend to be used.
    // For now, only the RocksDB engine is supported.
    //
    virtual
    void
    set_persistent_store(
        const std::uint16_t collocation_index,
        std::unique_ptr<rocksdb::DB> persistent_store) = 0;

    //
    // Inserts a single object into the data store.
    //
    virtual
    status::status_code
    insert_object(
        storage_engine_reference_handle* container_storage_engine_reference,
        const char* object_id,
        const byte_stream& object_data) = 0;

    //
    // Get an object from the data store.
    // Stores the object contents into the data stream if it exists.
    //
    virtual
    status::status_code
    get_object(
        storage_engine_reference_handle* container_storage_engine_reference,
        const char* object_id,
        byte_stream* object_data) = 0;

    //
    // Creates a new object container inside the data store.
    // Returns the associated column family reference on success.
    //
    virtual
    status::status_code
    create_container(
        const char* container_name,
        storage_engine_reference_handle** container_storage_engine_reference) = 0;

    //
    // Gets all the objects from a specified object container.
    // Returns them in an unordered fashion.
    //
    virtual
    status::status_code
    get_all_objects_from_container(
        storage_engine_reference_handle* container_storage_engine_reference,
        std::unordered_map<std::string, byte_stream>* objects) = 0;

    //
    // Closes the in-memory object container storage engine reference.
    //
    virtual
    status::status_code
    close_container_storage_engine_reference(
        storage_engine_reference_handle* container_storage_engine_reference) = 0;

    //
    // Removes an object from a given object container.
    //
    virtual
    status::status_code
    remove_object(
        storage_engine_reference_handle* container_storage_engine_reference,
        const char* object_id) = 0;

    //
    // Removes an object container.
    //
    virtual
    status::status_code
    remove_container(
        storage_engine_reference_handle* container_storage_engine_reference) = 0;

    //
    // Executes a batch of write operations (insert/remove) for objects.
    //
    virtual
    status::status_code
    execute_objects_write_batch(
        storage_engine_write_batch& write_batch) = 0;

    //
    // Registers an engine reference into the approved set of references.
    //
    virtual
    void
    register_approved_engine_references(
        const std::vector<storage_engine_reference_handle*> engine_references) = 0;

    //
    // Validates whether the provided engine reference is part
    // of the approved set of engine references for this engine instance.
    // Returns true if it is approved, false otherwise.
    //
    virtual
    bool
    fence_engine_reference(
        storage_engine_reference_handle* engine_reference) = 0;
};

} // namespace storage.
} // namespace lazarus.