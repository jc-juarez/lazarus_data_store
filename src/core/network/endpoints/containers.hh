// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'containers.hh'
// Author: jcjuarez
// Description:
//      Object container controller endpoints collection.
//      Handles CRUD operations for object containers.
// ****************************************************

#pragma once

#include "../server/server.hh"
#include <drogon/HttpController.h>

namespace lazarus
{
namespace network
{

class create_container_request_handler;
class remove_container_request_handler;

namespace endpoints
{

class containers : public drogon::HttpController<containers, false>
{
public:

    //
    // Endpoint constructor.
    //
    containers(
        std::unique_ptr<create_container_request_handler> create_container_request_handler,
        std::unique_ptr<remove_container_request_handler> remove_container_request_handler);

    METHOD_LIST_BEGIN
    METHOD_ADD(containers::create_container, "", drogon::Post);
    METHOD_ADD(containers::remove_container, "", drogon::Delete);
    METHOD_LIST_END

    //
    // Creates a new object container.
    //
    void
    create_container(
        const http_request& request,
        server_response_callback&& response_callback);

    //
    // Removes an object container from the data store and all of its contents.
    //
    void
    remove_container(
        const http_request& request,
        server_response_callback&& response_callback);

private:

    //
    // Container creation request handler.
    //
    std::unique_ptr<create_container_request_handler> create_container_request_handler_;

    //
    // Container removal request handler.
    //
    std::unique_ptr<remove_container_request_handler> remove_container_request_handler_;
};

} // namespace endpoints.
} // namespace network.
} // namespace lazarus.