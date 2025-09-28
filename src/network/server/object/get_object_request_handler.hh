// ****************************************************
// Lazarus Data Store
// Network
// 'get_object_request_handler.hh'
// Author: jcjuarez
// Description:
//      Manages the top-level orchestration for
//      object retrieval requests.
// ****************************************************

#pragma once

#include "object_request_handler.hh"

namespace lazarus
{
namespace network
{

class get_object_request_handler : public object_request_handler
{
public:

    //
    // Constructor.
    //
    get_object_request_handler(
        std::shared_ptr<storage::object_management_service> object_management_service);

    //
    // Handles the retrieval for an object.
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