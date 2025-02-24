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

namespace lazarus
{
namespace network
{

object_container_endpoint::object_container_endpoint(
    std::shared_ptr<lazarus::storage::data_store_accessor> data_store_accessor_handle)
    : data_store_accessor_{std::move(data_store_accessor_handle)}
{}

/*void
create_object_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{

}*/

void
object_container_endpoint::get_object_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)>&& callback)
{
    request->bodyData();
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(
        "Hello, this is a generic hello message from the SayHello "
        "controller");
    callback(resp);
}

} // namespace network.
} // namespace lazarus.