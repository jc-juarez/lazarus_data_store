// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'container_endpoint.cc'
// Author: jcjuarez
// Description:
//      Object container controller endpoints collection.
//      Handles CRUD operations for object containers.
// ****************************************************

#include "container_endpoint.hh"
#include "../server/request-handlers/container/create_container_request_handler.hh"
#include "../server/request-handlers/container/remove_container_request_handler.hh"

namespace lazarus
{
namespace network
{

container_endpoint::container_endpoint(
    std::unique_ptr<create_container_request_handler> create_container_request_handler,
    std::unique_ptr<remove_container_request_handler> remove_container_request_handler)
    : create_container_request_handler_{std::move(create_container_request_handler)},
      remove_container_request_handler_{std::move(remove_container_request_handler)}
{}

void
container_endpoint::create_container(
    const http_request& request,
    server_response_callback&& response_callback)
{
    create_container_request_handler_->run(
        request,
        std::move(response_callback));
}

void
container_endpoint::remove_container(
    const http_request& request,
    server_response_callback&& response_callback)
{
    remove_container_request_handler_->run(
        request,
        std::move(response_callback));
}

} // namespace network.
} // namespace lazarus.