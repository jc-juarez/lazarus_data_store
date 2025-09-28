// ****************************************************
// Lazarus Data Store
// Network
// 'container_endpoint.hh'
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

class container_endpoint : public drogon::HttpController<container_endpoint, false>
{
public:

    //
    // Endpoint constructor.
    //
    container_endpoint(
    std::unique_ptr<create_container_request_handler> create_container_request_handler,
    std::unique_ptr<remove_container_request_handler> remove_container_request_handler);

    METHOD_LIST_BEGIN
    METHOD_ADD(container_endpoint::create_container, "/", drogon::Put);
    METHOD_ADD(container_endpoint::remove_container, "/", drogon::Delete);
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

} // namespace network.
} // namespace lazarus.