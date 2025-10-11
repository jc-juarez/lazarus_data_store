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
    // Starts the storage engine.
    //
    virtual
    status::status_code
    start(
        const std::vector<std::string>& containers_names,
        std::unordered_map<std::string, storage_engine_reference_handle*>* storage_engine_references_mapping) = 0;

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
    // Fetches the existing object containers in the data store.
    //
    virtual
    status::status_code
    fetch_containers_from_disk(
        std::vector<std::string>* containers_names) = 0;

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
    // Removes an object container permanently from the filesystem.
    //
    virtual
    status::status_code
    remove_container(
        storage_engine_reference_handle* container_storage_engine_reference) = 0;
};

} // namespace storage.
} // namespace lazarus.