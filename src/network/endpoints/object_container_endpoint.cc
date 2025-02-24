// ****************************************************
// Lazarus Data Store
// Network
// 'object_container_endpoint.hh'
// Author: jcjuarez
// Description:
//      Object container controller endpoints collection.
//      Handles CRUD operations for object containers.
// ****************************************************

#include "object_container_endpoint.hh"
#include "../../storage/data_store_accessor.hh"
#include <thread>

namespace lazarus
{
namespace network
{

object_container_endpoint::object_container_endpoint(
    std::shared_ptr<lazarus::storage::data_store_accessor> data_store_accessor_handle)
    : data_store_accessor_{std::move(data_store_accessor_handle)}
{}

void
object_container_endpoint::create_object_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    //
    // Handle async insertion.
    // Response will be provided async.
    //
    data_store_accessor_->enqueue_async_object_insertion(
        "Joe",
        "My hobby is reading books!",
        std::move(callback));
}

void
object_container_endpoint::get_object_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)>&& callback)
{
    storage::byte_stream object_stream;

    //
    // Get contents from the db.
    //
    data_store_accessor_->get_object(
        "Joe",
        object_stream);

    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(
        object_stream.c_str());
    callback(resp);
}

} // namespace network.
} // namespace lazarus.