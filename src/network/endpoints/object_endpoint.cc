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
#include "object_endpoint.hh"
#include "../server/server.hh"
#include "../../storage/container.hh"
#include "../../storage/object_management_service.hh"
#include "../../storage/container_management_service.hh"
#include "../../schemas/request-interfaces/object_request.hh"

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
    schemas::object_request object_request{request};

    status::status_code status = object_management_service_->validate_object_operation_request(
        object_request);

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

    spdlog::info("Insert object request received. "
        "Optype={}, "
        "ObjectId={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_request.get_optype()),
        object_request.get_object_id(),
        object_request.get_object_container_name());

    std::shared_ptr<storage::container> object_container =
        object_management_service_->get_object_container_reference(object_request.get_object_container_name());

    //
    // If the object container is in deleted state, fail the operation.
    //
    const bool is_object_container_deleted =
        object_container != nullptr ? object_container->is_deleted() : false;

    if (object_container == nullptr ||
        is_object_container_deleted)
    {
        spdlog::error("Object container provided for insert object operation "
            "does not exist or is in deletion process. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "IsDeleted={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_object_container_name(),
            is_object_container_deleted);

        network::server::send_response(
            response_callback,
            status::object_container_not_exists);

        return;
    }

    //
    // At this point, it is guaranteed that ref-counted reference
    // to the object container is held, so enqueue a concurrent write
    // operation. The response will be provided asynchronously over the
    // provided callback if the enqueue operation is successful.
    //
    status = object_management_service_->orchestrate_concurrent_write_request(
        std::move(object_request),
        object_container,
        std::move(response_callback));

    if (status::failed(status))
    {
        spdlog::error("Failed to enqueue object insert request operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_object_container_name(),
            status);

        network::server::send_response(
            response_callback,
            status);
    }
}

void
object_endpoint::get_object(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{
    schemas::object_request object_request{request};

    status::status_code status = object_management_service_->validate_object_operation_request(
        object_request);

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

    spdlog::info("Read object request received. "
                 "Optype={}, "
                 "ObjectId={}, "
                 "ObjectContainerName={}.",
                 static_cast<std::uint8_t>(object_request.get_optype()),
                 object_request.get_object_id(),
                 object_request.get_object_container_name());

    std::shared_ptr<storage::container> object_container =
        object_management_service_->get_object_container_reference(object_request.get_object_container_name());

    //
    // If the object container is in deleted state, fail the operation.
    //
    const bool is_object_container_deleted =
        object_container != nullptr ? object_container->is_deleted() : false;

    if (object_container == nullptr ||
        is_object_container_deleted)
    {
        spdlog::error("Object container provided for get object operation "
            "does not exist or is in deletion process. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "IsDeleted={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_object_container_name(),
            is_object_container_deleted);

        network::server::send_response(
            response_callback,
            status::object_container_not_exists);

        return;
    }

    //
    // At this point, it is guaranteed that ref-counted reference
    // to the object container is held, so enqueue a concurrent write
    // operation. The response will be provided asynchronously over the
    // provided callback if the enqueue operation is successful.
    //
    status = object_management_service_->orchestrate_concurrent_read_request(
        std::move(object_request),
        object_container,
        std::move(response_callback));

    if (status::failed(status))
    {
        spdlog::error("Failed to enqueue object get request operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_object_container_name(),
            status);

        network::server::send_response(
            response_callback,
            status);
    }
}

void
object_endpoint::remove_object(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback)
{
    schemas::object_request object_request{request};

    status::status_code status = object_management_service_->validate_object_operation_request(
        object_request);

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

    spdlog::info("Remove object request received. "
        "Optype={}, "
        "ObjectId={}, "
        "ObjectContainerName={}.",
        static_cast<std::uint8_t>(object_request.get_optype()),
        object_request.get_object_id(),
        object_request.get_object_container_name());

    std::shared_ptr<storage::container> object_container =
        object_management_service_->get_object_container_reference(object_request.get_object_container_name());

    //
    // If the object container is in deleted state, fail the operation.
    //
    const bool is_object_container_deleted =
        object_container != nullptr ? object_container->is_deleted() : false;

    if (object_container == nullptr ||
        is_object_container_deleted)
    {
        spdlog::error("Object container provided for remove object operation "
            "does not exist or is in deletion process. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "IsDeleted={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_object_container_name(),
            is_object_container_deleted);

        network::server::send_response(
            response_callback,
            status::object_container_not_exists);

        return;
    }

    //
    // At this point, it is guaranteed that ref-counted reference
    // to the object container is held, so enqueue a concurrent write
    // operation. The response will be provided asynchronously over the
    // provided callback if the enqueue operation is successful.
    //
    status = object_management_service_->orchestrate_concurrent_write_request(
        std::move(object_request),
        object_container,
        std::move(response_callback));

    if (status::failed(status))
    {
        spdlog::error("Failed to enqueue object remove request operation. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_object_container_name(),
            status);

        network::server::send_response(
            response_callback,
            status);
    }
}

} // namespace network.
} // namespace lazarus.