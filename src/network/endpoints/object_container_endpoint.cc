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
#include "../../storage/data_store_service.hh"
#include "../../schemas/request-interfaces/object_container_request_interface.hh"

namespace lazarus
{
namespace network
{

object_container_endpoint::object_container_endpoint(
    std::shared_ptr<storage::data_store_service> data_store_service_handle)
    : data_store_service_{std::move(data_store_service_handle)}
{}

void
object_container_endpoint::create_object_container(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{
    schemas::object_container_request_interface object_container_request{
        request};

    spdlog::info("Create object container request received. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_container_request.get_optype()),
        object_container_request.get_name());

    const status::status_code status = data_store_service_->validate_object_container_operation_request(
        object_container_request);

    if (status::failed(status))
    {
        spdlog::error("Object container creation request validation failed. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_container_request.get_optype()),
            object_container_request.get_name(),
            status);

        network::server::send_response(
            response_callback,
            status);

        return;
    }

    //
    // Orchestrate the creation of the object
    // container in async serialized fashion.
    // Response will be provided by a separate thread.
    //
    data_store_service_->orchestrate_serial_object_container_operation(
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
    schemas::object_container_request_interface object_container_request{
        request};

    spdlog::info("Remove object container request received. "
        "Optype={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_container_request.get_optype()),
        object_container_request.get_name());

    const status::status_code status = data_store_service_->validate_object_container_operation_request(
        object_container_request);

    if (status::failed(status))
    {
        spdlog::error("Object container removal request validation failed. "
            "Optype={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_container_request.get_optype()),
            object_container_request.get_name(),
            status);

        network::server::send_response(
            response_callback,
            status);

        return;
    }

    //
    // Orchestrate the removal of the object
    // container in async serialized fashion.
    // Response will be provided by a separate thread.
    //
    data_store_service_->orchestrate_serial_object_container_operation(
        std::move(object_container_request),
        std::move(response_callback));
}

} // namespace network.
} // namespace lazarus.