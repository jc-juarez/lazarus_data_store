// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'insert_object_request_handler.hh'
// Author: jcjuarez
// Description:
//      Manages the top-level orchestration for
//      object insertion requests.
// ****************************************************

#pragma once

#include "object_request_handler.hh"

namespace lazarus
{
namespace network
{

class insert_object_request_handler : public object_request_handler
{
public:

    //
    // Constructor.
    //
    insert_object_request_handler(
        std::shared_ptr<storage::object_management_service> object_management_service);

    //
    // Handles the insertion for an object.
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
