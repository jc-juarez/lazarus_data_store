// ****************************************************
// Lazarus Data Store
// Storage
// 'data_store_service.hh'
// Author: jcjuarez
// Description:
//      Accessor core storage operations. 
// ****************************************************

#pragma once

#include <memory>
#include <optional>
#include <tbb/tbb.h>
#include <rocksdb/db.h>
#include <drogon/HttpController.h>
#include "../schemas/request-interfaces/object_container_request_interface.hh"

namespace lazarus
{
namespace storage
{

using byte = char;
using byte_stream = std::string;

class storage_engine;
class object_container_index;
class object_container_operation_serializer;

//
// Core storage access interface.
//
class data_store_service
{
public:

    //
    // Constructor data service.
    //
    data_store_service(
        std::shared_ptr<storage_engine> storage_engine_handle);

    //
    // Populates the in-memory contents of the object container
    // index based on the references received from the storage engine start.
    //
    void
    populate_object_container_index(
        const std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>& column_family_references_mapping);

    //
    // Creates the root metadata column
    // families for the system if not present already.
    // On first-time startup, the data store will create them.
    //
    void
    create_internal_metadata_column_families();

    //
    // Inserts a single object into the data store in async fashion.
    // Ensures that a copy of the object is created before enqueueing the async
    // task, and notifies back the result of the operation over the provided callback.
    //
    void
    enqueue_async_object_insertion(
        const char* object_id,
        const byte* object_data_stream,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    //
    // Get an object from the data store.
    // Stores the object contents into the data stream if it exists.
    //
    void
    get_object(
        const char* object_id,
        byte_stream& object_data_stream);

    //
    // Orchestrates possible update operations to the object container index.
    // Given possible check-then-act race conditions upon object
    // container creation/deletion, these operations need to be serialized.
    // Handles callback response.
    //
    void
    orchestrate_serial_object_container_operation(
        lazarus::schemas::object_container_request_interface&& object_container_request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    //
    // Checks if an object container exists in the object container index.
    //
    bool
    object_container_exists(
        const char* object_container_name);

private:

    //
    // Gets the object containers internal
    // metadata column family association pair if it exists.
    //
    std::optional<std::pair<std::string, rocksdb::ColumnFamilyHandle*>>
    find_object_containers_internal_metadata_association_pair(
        const std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>& column_family_references_mapping);

    //
    // Object insertion dispatcher entry point.
    // Corresponds to the upcall for executing an insertion action.
    //
    void
    object_insertion_dispatch_proxy(
        const std::string&& object_id,
        const byte_stream&& object_data_stream,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    //
    // Handle for the storage enine.
    //
    std::shared_ptr<storage_engine> storage_engine_;

    //
    // Handle for the object container index component.
    //
    std::shared_ptr<object_container_index> object_container_index_;

    //
    // Handle for the object container operation serializer component.
    //
    std::shared_ptr<object_container_operation_serializer> object_container_operation_serializer_;

    //
    // Scalable thread pool for handling async object insertions.
    //
    tbb::task_arena object_insertion_thread_pool_;
};

} // namespace storage.
} // namespace lazarus.