// ****************************************************
// Lazarus Data Store
// Network
// 'object_container_endpoint.hh'
// Author: jcjuarez
// Description:
//      Object container controller endpoints collection.
//      Handles CRUD operations for object containers.
// ****************************************************

#pragma once

#include <drogon/HttpController.h>

/*namespace lazarus
{
namespace network
{*/

class object_container_endpoint : public drogon::HttpController<object_container_endpoint, false>
{
  public:

    object_container_endpoint(const std::string &str);

    METHOD_LIST_BEGIN
    //
    // Gets the metadata of a given element container.
    //
    METHOD_ADD(object_container_endpoint::get_object_container, "/", drogon::Get);
    METHOD_LIST_END

    void
    get_object_container(
      const drogon::HttpRequestPtr& request,
      std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

/*} // namespace network.
} // namespace lazarus.*/