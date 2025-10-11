// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Network
// 'objects.hh'
// Author: jcjuarez
// Description:
//      Object controller endpoints collection.
//      Handles CRUD operations for objects.
// ****************************************************

#pragma once

#include "../server/server.hh"
#include <drogon/HttpController.h>

namespace lazarus
{
namespace network
{

class insert_object_request_handler;
class get_object_request_handler;
class remove_object_request_handler;

namespace endpoints
{

class objects : public drogon::HttpController<objects, false>
{
public:

    //
    // Endpoint constructor.
    //
    objects(
        std::unique_ptr<insert_object_request_handler> insert_object_request_handler,
        std::unique_ptr<get_object_request_handler> get_object_request_handler,
        std::unique_ptr<remove_object_request_handler> remove_object_request_handler);

    METHOD_LIST_BEGIN
    METHOD_ADD(objects::insert_object, "", drogon::Post);
    METHOD_ADD(objects::get_object, "", drogon::Get);
    METHOD_ADD(objects::remove_object, "", drogon::Delete);
    METHOD_LIST_END

    //
    // Inserts an object into an object container.
    //
    void
    insert_object(
        const http_request& request,
        server_response_callback&& response_callback);

    //
    // Retrieves an object from an object container.
    //
    void
    get_object(
        const http_request& request,
        server_response_callback&& response_callback);

    //
    // Removes an object from an object container.
    //
    void
    remove_object(
        const http_request& request,
        server_response_callback&& response_callback);

private:

    //
    // Object insertion request handler.
    //
    std::unique_ptr<insert_object_request_handler> insert_object_request_handler_;

    //
    // Object retrieval request handler.
    //
    std::unique_ptr<get_object_request_handler> get_object_request_handler_;

    //
    // Object removal request handler.
    //
    std::unique_ptr<remove_object_request_handler> remove_object_request_handler_;
};

} // namespace endpoints.
} // namespace network.
} // namespace lazarus.