// ****************************************************
// Lazarus Data Store
// Storage
// 'container_operation_serializer.hh'
// Author: jcjuarez
// Description:
//      Manages object container operations
//      in a serialized manner. 
// ****************************************************

#pragma once

#include <memory>
#include "../status/status.hh"
#include "../network/server/server.hh"
#include "../common/task_serializer.hh"
#include "../schemas/request-interfaces/container_request.hh"

namespace lazarus
{
namespace storage
{

class storage_engine;
class container_index;

//
// Serialized object container operation orchestrator.
//
class container_operation_serializer
{
public:

    //
    // Constructor for the object operation serializer.
    //
    container_operation_serializer(
        std::shared_ptr<storage_engine> storage_engine_handle,
        std::shared_ptr<container_index> container_index);

    //
    // Enqueues a serialized object container operation into the serializer.
    // Handles server response callback replies in an asynchronous manner.
    //
    void
    enqueue_object_container_operation(
        schemas::container_request&& object_container_request,
        network::server_response_callback&& response_callback);

private:

    //
    // Object container operation serialization entry point.
    // All calls to this proxy are serialized.
    //
    void
    object_container_operation_serial_proxy(
        const schemas::container_request& object_container_request,
        const network::server_response_callback& response_callback);

    //
    // Orchestrates the object container creation process.
    //
    status::status_code
    handle_object_container_creation(
        const schemas::container_request& object_container_request);

    //
    // Orchestrates the object container removal process.
    //
    status::status_code
    handle_object_container_removal(
        const schemas::container_request& object_container_request);

    //
    // Serializer task queue for executing object container operations serially.
    //
    common::task_serializer object_container_operations_serializer_;

    //
    // Reference for the storage engine component.
    //
    std::shared_ptr<storage_engine> storage_engine_;

    //
    // Reference for the object container index component.
    //
    std::shared_ptr<container_index> object_container_index_;
};

} // namespace storage.
} // namespace lazarus.