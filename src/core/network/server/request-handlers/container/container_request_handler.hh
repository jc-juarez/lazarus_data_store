// ****************************************************
// Lazarus Data Store
// Network
// 'container_request_handler.hh'
// Author: jcjuarez
// Description:
//      Base class for top-level container operations
//      orchestration.
// ****************************************************

#pragma once

#include <memory>
#include "../../../../common/aliases.hh"
#include "../../../../schemas/request-interfaces/container_request.hh"

namespace lazarus
{

namespace storage
{
class container_management_service;
}

namespace network
{

class container_request_handler
{
public:

    //
    // Constructor.
    //
    container_request_handler(
        std::shared_ptr<storage::container_management_service> container_management_service);

    // Middleware execution for the container operation.
    // Handles common validations and actions for the HTTP request.
    void
    run(
        const http_request& request,
        server_response_callback&& response_callback);

    //
    // Handles the operation for the container.
    // Must be implemented by derived classes.
    //
    virtual
    void
    execute_operation(
        schemas::container_request&& container_request,
        server_response_callback&& response_callback) = 0;

protected:

    //
    // Handle for the container management service.
    //
    std::shared_ptr<storage::container_management_service> container_management_service_;
};

} // namespace network.
} // namespace lazarus.