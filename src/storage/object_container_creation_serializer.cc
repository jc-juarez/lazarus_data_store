// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container_creation_serializer.cc'
// Author: jcjuarez
// Description:
//      Manages the object container creation
//      process in a serialized manner. 
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "object_container_index.hh"
#include "object_container_creation_serializer.hh"

namespace lazarus
{
namespace storage
{

object_container_creation_serializer::object_container_creation_serializer(
    std::shared_ptr<storage_engine> storage_engine_handle,
    std::shared_ptr<object_container_index> object_cotainer_index_handle)
    : storage_engine_{std::move(storage_engine_handle)},
      object_container_index_{std::move(object_cotainer_index_handle)},
      serializer_queue_{1u} // Must always be a single-threaded task queue.
{}

void
object_container_creation_serializer::enqueue_object_container_creation(
    lazarus::schemas::object_container_request_interface&& object_container_request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    //
    // Enqueue the async object container creation action.
    // This is a single-threaded execution for serialization purposes.
    //
    serializer_queue_.execute(
        [this,
        object_container_request = std::move(object_container_request),
        callback = std::move(callback)]() mutable
        {
            this->object_container_creation_serial_proxy(
                std::move(object_container_request),
                std::move(callback));
        });
}

void
object_container_creation_serializer::object_container_creation_serial_proxy(
    const lazarus::schemas::object_container_request_interface&& object_container_request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    spdlog::info("Executing serialized object container creation action. "
        "ObjectContainerId={}.",
        object_container_request.get_id());
}

} // namespace storage.
} // namespace lazarus.