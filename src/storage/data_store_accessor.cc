// ****************************************************
// Lazarus Data Store
// Storage
// 'data_store_accessor.cc'
// Author: jcjuarez
// Description:
//      Accessor core storage operations. 
// ****************************************************

#include "storage_engine.hh"
#include "data_store_accessor.hh"
#include "object_container_index.hh"
#include "object_container_creation_serializer.hh"

namespace lazarus
{
namespace storage
{

data_store_accessor::data_store_accessor(
    std::shared_ptr<storage_engine> storage_engine_handle)
    : storage_engine_(std::move(storage_engine_handle)),
      object_insertion_thread_pool_{16u}
{
    //
    // Object container index component allocation.
    //
    object_container_index_ = std::make_shared<object_container_index>();

    //
    // Object container creation serializer component allocation.
    //
    object_container_creation_serializer_ = std::make_shared<object_container_creation_serializer>(
        storage_engine_,
        object_container_index_);
}

void
data_store_accessor::enqueue_async_object_insertion(
    const char* object_id,
    const byte* object_data_stream,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    //
    // Ensure the associated contents of the object are copied before proceeding. 
    //
    std::string object_id_to_dispatch{object_id};
    byte_stream object_data_stream_to_dispatch{object_data_stream};

    //
    // Enqueue the object insertion action.
    //
    object_insertion_thread_pool_.execute(
        [this,
        object_id_to_dispatch = std::move(object_id_to_dispatch),
        object_data_stream_to_dispatch = std::move(object_data_stream_to_dispatch),
        callback = std::move(callback)]() mutable
        {
            this->object_insertion_dispatch_proxy(
                std::move(object_id_to_dispatch),
                std::move(object_data_stream_to_dispatch),
                std::move(callback));
        });
}

void
data_store_accessor::get_object(
    const char* object_id,
    byte_stream& object_data_stream)
{
    storage_engine_->get_object(
        object_id,
        object_data_stream);
}

void
data_store_accessor::object_insertion_dispatch_proxy(
    const std::string&& object_id,
    const byte_stream&& object_data_stream,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    //
    // At this point, it is guaranteed that the memory contents
    // of the object are safely copied ino the provided containers.
    //
    storage_engine_->insert_object(
        object_id.c_str(),
        object_data_stream.c_str());

    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(
        "Async response. Object has been inserted.");
    
    callback(resp);
}

} // namespace storage.
} // namespace lazarus.