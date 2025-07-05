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
#include "../../storage/object_management_service.hh"

namespace lazarus
{
namespace network
{

object_endpoint::object_endpoint(
    std::shared_ptr<storage::object_management_service> object_management_service)
    : object_management_service_{std::move(object_management_service)}
{}

void
object_endpoint::insert_object(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{
    schemas::object_request_interface object_request{
        request};

    spdlog::info("Insert object request received. "
        "Optype={}, "
        "ObjectId={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_request.get_optype()),
        object_request.get_object_id(),
        object_request.get_object_container_name());

    const status::status_code status = object_management_service_->validate_object_operation_request(
        object_request);
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