// ****************************************************
// Lazarus Data Store
// Network
// 'object_request_handler.hh'
// Author: jcjuarez
// Description:
//      Base class for top-level object operations
//      orchestration.
// ****************************************************

#pragma once

#include <memory>
#include "../../../../common/aliases.hh"
#include "../../../../schemas/request-interfaces/object_request.hh"

namespace lazarus
{

namespace storage
{
class container;
class object_management_service;
}

namespace network
{

class object_request_handler
{
public:

    //
    // Constructor.
    //
    object_request_handler(
        std::shared_ptr<storage::object_management_service> object_management_service);

    // Middleware execution for the object operation.
    // Handles common validations and actions for the HTTP request.
    void
    run(
        const http_request& request,
        server_response_callback&& response_callback);

    //
    // Handles the operation for the object.
    // Must be implemented by derived classes.
    //
    virtual
    void
    execute_operation(
        schemas::object_request&& object_request,
        std::shared_ptr<storage::container> container,
        server_response_callback&& response_callback) = 0;

protected:

    //
    // Handle for the object management service.
    //
    std::shared_ptr<storage::object_management_service> object_management_service_;
};

} // namespace network.
} // namespace lazarus.