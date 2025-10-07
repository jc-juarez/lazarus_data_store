// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'object_request_handler.cc'
// Author: jcjuarez
// Description:
//      Base class for top-level object operations
//      orchestration.
// ****************************************************

#include "../../server.hh"
#include <spdlog/spdlog.h>
#include "object_request_handler.hh"
#include "../../../../storage/container.hh"
#include "../../../../storage/object_management_service.hh"

namespace lazarus
{
namespace network
{

object_request_handler::object_request_handler(
    std::shared_ptr<storage::object_management_service> object_management_service)
    : object_management_service_{std::move(object_management_service)}
{}

void
object_request_handler::run(
    const http_request& request,
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

    std::shared_ptr<storage::container> container =
        object_management_service_->get_container_reference(object_request.get_container_name());

    //
    // If the object container is in deleted state, fail the operation.
    //
    const bool is_container_deleted =
        container != nullptr ? container->is_deleted() : false;

    if (container == nullptr ||
        is_container_deleted)
    {
        spdlog::error("Object container provided for object operation "
            "does not exist or is in deletion process. "
            "Optype={}, "
            "ObjectId={}, "
            "ObjectContainerName={}, "
            "IsDeleted={}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id(),
            object_request.get_container_name(),
            is_container_deleted);

        network::server::send_response(
            response_callback,
            status::container_not_exists);

        return;
    }

    //
    // Request is valid.
    // Delegate the action to the corresponding request handler.
    //
    execute_operation(
        std::move(object_request),
        std::move(container),
        std::move(response_callback));
}

} // namespace network.
} // namespace lazarus.