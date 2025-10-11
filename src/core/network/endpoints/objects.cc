// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'objects.cc'
// Author: jcjuarez
// Description:
//      Object controller endpoints collection.
//      Handles CRUD operations for objects.
// ****************************************************

#include "objects.hh"
#include "../server/request-handlers/object/get_object_request_handler.hh"
#include "../server/request-handlers/object/insert_object_request_handler.hh"
#include "../server/request-handlers/object/remove_object_request_handler.hh"

namespace lazarus
{
namespace network
{
namespace endpoints
{

objects::objects(
    std::unique_ptr<insert_object_request_handler> insert_object_request_handler,
    std::unique_ptr<get_object_request_handler> get_object_request_handler,
    std::unique_ptr<remove_object_request_handler> remove_object_request_handler)
    : insert_object_request_handler_{std::move(insert_object_request_handler)},
      get_object_request_handler_{std::move(get_object_request_handler)},
      remove_object_request_handler_{std::move(remove_object_request_handler)}
{}

void
objects::insert_object(
    const http_request& request,
    server_response_callback&& response_callback)
{
    insert_object_request_handler_->run(
        request,
        std::move(response_callback));
}

void
objects::get_object(
    const http_request& request,
    server_response_callback&& response_callback)
{
    get_object_request_handler_->run(
        request,
        std::move(response_callback));
}

void
objects::remove_object(
    const http_request& request,
    server_response_callback&& response_callback)
{
    remove_object_request_handler_->run(
        request,
        std::move(response_callback));
}

} // namespace endpoints.
} // namespace network.
} // namespace lazarus.