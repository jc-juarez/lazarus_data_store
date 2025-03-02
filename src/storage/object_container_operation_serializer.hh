// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container_operation_serializer.hh'
// Author: jcjuarez
// Description:
//      Manages object container operations
//      in a serialized manner. 
// ****************************************************

#pragma once

#include <memory>
#include <tbb/tbb.h>
#include "../schemas/request-interfaces/object_container_request_interface.hh"

namespace lazarus
{
namespace storage
{

class storage_engine;
class object_container_index;

//
// Serialized object container operation orchestrator.
//
class object_container_operation_serializer
{
public:

    //
    // Constructor for the object operation serializer.
    //
    object_container_operation_serializer(
        std::shared_ptr<storage_engine> storage_engine_handle,
        std::shared_ptr<object_container_index> object_cotainer_index_handle);

    //
    // Enqueues a serialized object container operation into the serializer.
    // Handles server callback replies in an asynchronous manner.
    //
    void
    enqueue_object_container_operation(
        lazarus::schemas::object_container_request_interface&& object_container_request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:

    //
    // Object container operation serialization entry point.
    // All calls to this proxy are serialized.
    //
    void
    object_container_operation_serial_proxy(
        const lazarus::schemas::object_container_request_interface&& object_container_request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    //
    // Serializer task queue for executing object container operations serially.
    // Implemented using a single threaded thread pool.
    //
    tbb::task_arena serializer_queue_;

    //
    // Reference for the storage engine component.
    //
    std::shared_ptr<storage_engine> storage_engine_;

    //
    // Reference for the object container index component.
    //
    std::shared_ptr<object_container_index> object_container_index_;
};

} // namespace storage.
} // namespace lazarus.