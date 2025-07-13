// ****************************************************
// Lazarus Data Store
// Network
// 'container_endpoint.hh'
// Author: jcjuarez
// Description:
//      Object container controller endpoints collection.
//      Handles CRUD operations for object containers.
// ****************************************************

#include <spdlog/spdlog.h>
#include "../server/server.hh"
#include "container_endpoint.hh"
#include "../../storage/container_management_service.hh"
#include "../../schemas/request-interfaces/container_request.hh"

namespace lazarus
{
namespace network
{

container_endpoint::container_endpoint(
    std::shared_ptr<storage::container_management_service> container_management_service_handle)
    : container_management_service_{std::move(container_management_service_handle)}
{}

void
container_endpoint::create_container(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{
    schemas::container_request container_request{request};

    const status::status_code status = container_management_service_->validate_container_operation_request(
        container_request);

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
        static_cast<std::uint8_t>(container_request.get_optype()),
        container_request.get_name());

    //
    // Orchestrate the creation of the object
    // container in async serialized fashion.
    // Response will be provided by a separate thread.
    //
    container_management_service_->orchestrate_serial_container_operation(
        std::move(container_request),
        std::move(response_callback));
}

void
container_endpoint::get_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)>&& response_callback)
{}

void
container_endpoint::remove_container(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{
    schemas::container_request container_request{request};

    const status::status_code status = container_management_service_->validate_container_operation_request(
        container_request);

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
        static_cast<std::uint8_t>(container_request.get_optype()),
        container_request.get_name());

    //
    // Orchestrate the removal of the object
    // container in async serialized fashion.
    // Response will be provided by a separate thread.
    //
    container_management_service_->orchestrate_serial_container_operation(
        std::move(container_request),
        std::move(response_callback));
}

} // namespace network.
} // namespace lazarus.