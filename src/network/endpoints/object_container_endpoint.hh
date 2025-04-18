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

#include "../server/server.hh"
#include <drogon/HttpController.h>

namespace lazarus
{

namespace storage
{
class object_container_management_service;
}

namespace network
{

class object_container_endpoint : public drogon::HttpController<object_container_endpoint, false>
{
public:

  //
  // Endpoint constructor.
  //
  object_container_endpoint(
    std::shared_ptr<storage::object_container_management_service> object_container_management_service_handle);

  METHOD_LIST_BEGIN
  METHOD_ADD(object_container_endpoint::create_object_container, "/", drogon::Put);
  METHOD_ADD(object_container_endpoint::get_object_container, "/", drogon::Get);
  METHOD_ADD(object_container_endpoint::remove_object_container, "/", drogon::Delete);
  METHOD_LIST_END

  //
  // Creates a new object container.
  //
  void
  create_object_container(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback);

  //
  // Gets the metadata of a given object container.
  //
  void
  get_object_container(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback);

  //
  // Removes an object container from the data store and all of its contents.
  //
  void
  remove_object_container(
    const drogon::HttpRequestPtr& request,
    server_response_callback&& response_callback);

private:

  //
  // Object container management service handle.
  //
  std::shared_ptr<storage::object_container_management_service> object_container_management_service_;
};

} // namespace network.
} // namespace lazarus.