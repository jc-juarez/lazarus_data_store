// ****************************************************
// Lazarus Data Store
// Schemas
// 'object_container_request_interface.hh'
// Author: jcjuarez
// Description:
//      Structure for defining an object container
//      from a network request. 
// ****************************************************

#pragma once

#include <string>
#include <drogon/HttpController.h>

namespace lazarus
{
namespace schemas
{

//
// Structure for holding the request
// data for an object container.
//
class object_container_request_interface
{
public:

    //
    // Constructor for the request interface.
    //
    object_container_request_interface(
        const drogon::HttpRequestPtr& request);

    //
    // Move constructor for the request interface.
    //
    object_container_request_interface(
        object_container_request_interface&& other);

    //
    // Gets the object container identifier.
    //
    const char*
    get_id() const;


private:

    //
    // Object container identifier.
    //
    std::string id_;

    //
    // Expected object container identifier key name in the client request.
    //
    static constexpr const char* id_key_name = "id";
};

} // namespace schemas.
} // namespace lazarus.