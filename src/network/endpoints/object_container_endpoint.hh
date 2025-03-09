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

namespace storage
{
  class data_store_service;
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
    std::shared_ptr<storage::data_store_service> data_store_service_handle);

  METHOD_LIST_BEGIN
  METHOD_ADD(object_container_endpoint::create_object_container, "/", drogon::Put);
  METHOD_ADD(object_container_endpoint::get_object_container, "/", drogon::Get);
  METHOD_LIST_END

  //
  // Creates a new object container.
  //
  void
  create_object_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

  //
  // Gets the metadata of a given object container.
  //
  void
  get_object_container(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:

  //
  // Data store service handle.
  //
  std::shared_ptr<storage::data_store_service> data_store_service_;
};

} // namespace network.
} // namespace lazarus.