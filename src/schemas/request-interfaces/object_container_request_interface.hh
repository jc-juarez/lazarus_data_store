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
#include <cstdint>
#include <drogon/HttpController.h>
#include "../request-optypes/object_container_request_optype.hh"

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
    // Gets the object container name.
    //
    const char*
    get_name() const;

    //
    // Gets the object container request optype.
    //
    object_container_request_optype
    get_optype() const;


private:

    //
    // Object container name.
    //
    std::string name_;

    //
    // Expected object container name key tag in the client request.
    //
    static constexpr const char* name_key_tag = "name";

    //
    // Optype for the object container operation.
    //
    object_container_request_optype optype_;
};

} // namespace schemas.
} // namespace lazarus.