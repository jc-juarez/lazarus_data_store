// ****************************************************
// Lazarus Data Store
// Network
// 'object_endpoint.hh'
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

namespace storage
{
class frontline_cache;
class container_management_service;
}

namespace network
{

class object_endpoint : public drogon::HttpController<object_endpoint, false>
{
public:

    //
    // Endpoint constructor.
    //
    object_endpoint(
        std::shared_ptr<storage::frontline_cache> frontline_cache,
        std::shared_ptr<storage::object_management_service> object_management_service);

    METHOD_LIST_BEGIN
    METHOD_ADD(object_endpoint::insert_object, "/", drogon::Post);
    METHOD_ADD(object_endpoint::get_object, "/", drogon::Get);
    METHOD_ADD(object_endpoint::remove_object, "/", drogon::Delete);
    METHOD_LIST_END

    //
    // Inserts an object into an object container.
    //
    void
    insert_object(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback);

    //
    // Retrieves an object from an object container.
    //
    void
    get_object(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback);

    //
    // Removes an object from an object container.
    //
    void
    remove_object(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback);

private:

    //
    // Object management service handle.
    //
    std::shared_ptr<storage::object_management_service> object_management_service_;

    //
    // Frontline cache handle.
    //
    std::shared_ptr<storage::frontline_cache> frontline_cache_;
};

} // namespace network.
} // namespace lazarus.