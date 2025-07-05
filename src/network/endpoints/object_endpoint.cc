// ****************************************************
// Lazarus Data Store
// Network
// 'object_endpoint.cc'
// Author: jcjuarez
// Description:
//      Object controller endpoints collection.
//      Handles CRUD operations for objects.
// ****************************************************

#include <spdlog/spdlog.h>
#include "../server/server.hh"
#include "object_endpoint.hh"
#include "../../storage/object_container_management_service.hh"
#include "../../schemas/request-interfaces/object_request_interface.hh"

namespace lazarus
{
namespace network
{

object_endpoint::object_endpoint(
    std::shared_ptr<storage::object_container_management_service> object_container_management_service_handle)
    : object_container_management_service_{std::move(object_container_management_service_handle)}
{}

void
object_endpoint::insert_object(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{
    spdlog::info("Insert object request received.");
}

void
object_endpoint::get_object(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{}

void
object_endpoint::remove_object(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{}

} // namespace network.
} // namespace lazarus.