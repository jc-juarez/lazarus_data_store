// ****************************************************
// Lazarus Data Store
// Network
// 'remove_object_request_handler.hh'
// Author: jcjuarez
// Description:
//      Manages the top-level orchestration for
//      object removal requests.
// ****************************************************

#pragma once

#include "object_request_handler.hh"

namespace lazarus
{
namespace network
{

class remove_object_request_handler : public object_request_handler
{
public:

    //
    // Constructor.
    //
    remove_object_request_handler(
        std::shared_ptr<storage::object_management_service> object_management_service);

    //
    // Handles the removal for an object.
    //
    virtual
    void
    execute_operation(
        schemas::object_request&& object_request,
        std::shared_ptr<storage::container> container,
        server_response_callback&& response_callback) override final;
};

} // namespace network.
} // namespace lazarus.