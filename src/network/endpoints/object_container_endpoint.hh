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

namespace lazarus
{
namespace network
{

class object_container_endpoint : public drogon::HttpController<object_container_endpoint, false>
{
public:

  object_container_endpoint(const std::string &str);

  METHOD_LIST_BEGIN
  //METHOD_ADD(object_container_endpoint::create_object_container, "/", drogon::Put);
  METHOD_ADD(object_container_endpoint::get_object_container, "/", drogon::Get);
  METHOD_LIST_END

  //
  // Creates a new object container.
  //
  /*void
  create_object_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback);*/

  //
  // Gets the metadata of a given object container.
  //
  void
  get_object_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

} // namespace network.
} // namespace lazarus.