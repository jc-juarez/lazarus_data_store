// ****************************************************
// Lazarus Data Store
// Network
// 'remove_container_request_handler.hh'
// Author: jcjuarez
// Description:
//      Manages the top-level orchestration for
//      container removal requests.
// ****************************************************

#pragma once

#include "container_request_handler.hh"

namespace lazarus
{
namespace network
{

class remove_container_request_handler : public container_request_handler
{
public:

    //
    // Constructor.
    //
    remove_container_request_handler(
        std::shared_ptr<storage::container_management_service> container_management_service);

    //
    // Handles the removal for a container.
    //
    virtual
    void
    execute_operation(
        schemas::container_request&& container_request,
        server_response_callback&& response_callback) override final;
};

} // namespace network.
} // namespace lazarus.