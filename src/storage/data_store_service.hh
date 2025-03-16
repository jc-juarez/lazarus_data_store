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
#include "../status/status.hh"
#include "../common/aliases.hh"
#include <drogon/HttpController.h>
#include "../network/server/server.hh"
#include "../schemas/request-interfaces/object_container_request_interface.hh"

namespace lazarus
{
namespace storage
{

class storage_engine;
class garbage_collector;
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
    status::status_code
    populate_object_container_index(
        std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>* storage_engine_references_mapping);

    //
    // Inserts a single object into the data store in async fashion.
    // Ensures that a copy of the object is created before enqueueing the async
    // task, and notifies back the result of the operation over the provided response_callback.
    //
    void
    enqueue_async_object_insertion(
        const char* object_id,
        const byte* object_data_stream,
        network::server_response_callback&& response_callback);

    //
    // Get an object from the data store.
    // Stores the object contents into the data stream if it exists.
    //
    void
    get_object(
        const char* object_id,
        byte_stream& object_data);

    //
    // Orchestrates possible update operations to the object container index.
    // Given possible check-then-act race conditions upon object
    // container creation/deletion, these operations need to be serialized.
    // Handles callback response.
    //
    void
    orchestrate_serial_object_container_operation(
        schemas::object_container_request_interface&& object_container_request,
        network::server_response_callback&& response_callback);

    //
    // Validates if an object container operation request.
    //
    status::status_code
    validate_object_container_operation_request(
        const schemas::object_container_request_interface& object_container_request);

private:

    //
    // Creates the root metadata column
    // families for the system if not present already.
    // On first-time startup, the data store will create them.
    //
    status::status_code
    create_internal_metadata_column_families(
        std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>* storage_engine_references_mapping);

    //
    // Object insertion dispatcher entry point.
    // Corresponds to the upcall for executing an insertion action.
    //
    void
    object_insertion_dispatch_proxy(
        const std::string&& object_id,
        const byte_stream&& object_data_stream,
        network::server_response_callback&& response_callback);

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

    //
    // Handle for the garbage collector.
    //
    std::unique_ptr<garbage_collector> garbage_collector_;
};

} // namespace storage.
} // namespace lazarus.