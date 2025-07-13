// ****************************************************
// Lazarus Data Store
// Network
// 'object_container_endpoint.hh'
// Author: jcjuarez
// Description:
//      Object container controller endpoints collection.
//      Handles CRUD operations for object containers.
// ****************************************************

#include <spdlog/spdlog.h>
#include "../server/server.hh"
#include "object_container_endpoint.hh"
#include "../../storage/container_management_service.hh"
#include "../../schemas/request-interfaces/object_container_request_interface.hh"

namespace lazarus
{
namespace network
{

object_container_endpoint::object_container_endpoint(
    std::shared_ptr<storage::container_management_service> object_container_management_service_handle)
    : object_container_management_service_{std::move(object_container_management_service_handle)}
{}

void
object_container_endpoint::create_object_container(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{
    schemas::object_container_request_interface object_container_request{request};

    const status::status_code status = object_container_management_service_->validate_object_container_operation_request(
        object_container_request);

    if (status::failed(status))
    {
        //
        // Request validation failed. Do not log the request parameters
        // here as to avoid logging potentially malformed parameters.
        // The required logging should be taken care of by the management service.
        //
        network::server::send_response(
            response_callback,
            status);

        return;
    }

    spdlog::info("Create object container request received. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_container_request.get_optype()),
        object_container_request.get_name());

    //
    // Orchestrate the creation of the object
    // container in async serialized fashion.
    // Response will be provided by a separate thread.
    //
    object_container_management_service_->orchestrate_serial_object_container_operation(
        std::move(object_container_request),
        std::move(response_callback));
}

void
object_container_endpoint::get_object_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)>&& response_callback)
{}

void
object_container_endpoint::remove_object_container(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{
    schemas::object_container_request_interface object_container_request{request};

    const status::status_code status = object_container_management_service_->validate_object_container_operation_request(
        object_container_request);

    if (status::failed(status))
    {
        //
        // Request validation failed. Do not log the request parameters
        // here as to avoid logging potentially malformed parameters.
        // The required logging should be taken care of by the management service.
        //
        network::server::send_response(
            response_callback,
            status);

        return;
    }

    spdlog::info("Remove object container request received. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_container_request.get_optype()),
        object_container_request.get_name());

    //
    // Orchestrate the removal of the object
    // container in async serialized fashion.
    // Response will be provided by a separate thread.
    //
    object_container_management_service_->orchestrate_serial_object_container_operation(
        std::move(object_container_request),
        std::move(response_callback));
}

} // namespace network.
} // namespace lazarus.