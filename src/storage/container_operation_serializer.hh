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

class container_index;
class storage_engine_interface;

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
        std::shared_ptr<storage_engine_interface> storage_engine_handle,
        std::shared_ptr<container_index> container_index);

    //
    // Enqueues a serialized object container operation into the serializer.
    // Handles server response callback replies in an asynchronous manner.
    //
    void
    enqueue_container_operation(
        schemas::container_request&& container_request,
        network::server_response_callback&& response_callback);

private:

    //
    // Object container operation serialization entry point.
    // All calls to this proxy are serialized.
    //
    void
    container_operation_serial_proxy(
        const schemas::container_request& container_request,
        const network::server_response_callback& response_callback);

    //
    // Orchestrates the object container creation process.
    //
    status::status_code
    handle_container_creation(
        const schemas::container_request& container_request);

    //
    // Orchestrates the object container removal process.
    //
    status::status_code
    handle_container_removal(
        const schemas::container_request& container_request);

    //
    // Serializer task queue for executing object container operations serially.
    //
    common::task_serializer container_operations_serializer_;

    //
    // Reference for the storage engine component.
    //
    std::shared_ptr<storage_engine_interface> storage_engine_;

    //
    // Reference for the object container index component.
    //
    std::shared_ptr<container_index> container_index_;
};

} // namespace storage.
} // namespace lazarus.