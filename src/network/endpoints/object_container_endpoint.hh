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

class object_container_endpoint : public drogon::HttpController<object_container_endpoint>
{
  public:

    //
    // Explicit manual instantiation.
    //
    static constexpr bool isAutoCreation = false;

    explicit object_container_endpoint(int data);

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