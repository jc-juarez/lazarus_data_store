// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'container_request_handler.cc'
// Author: jcjuarez
// Description:
//      Base class for top-level container operations
//      orchestration.
// ****************************************************

#include "../../server.hh"
#include "container_request_handler.hh"
#include "../../../../storage/container_management_service.hh"

namespace lazarus
{
namespace network
{

container_request_handler::container_request_handler(
    std::shared_ptr<storage::container_management_service> container_management_service)
    : container_management_service_{std::move(container_management_service)}
{}

void
container_request_handler::run(
    const http_request& request,
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

    //
    // Request is valid.
    // Delegate the action to the corresponding request handler.
    //
    execute_operation(
        std::move(container_request),
        std::move(response_callback));
}

} // namespace network.
} // namespace lazarus.