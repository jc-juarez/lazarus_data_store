// ****************************************************
// Lazarus Data Store
// Schemas
// 'container_request.hh'
// Author: jcjuarez
// Description:
//      Structure for defining an object container
//      from a network request. 
// ****************************************************

#pragma once

#include <string>
#include <cstdint>
#include <drogon/HttpController.h>
#include "../request-optypes/container_request_optype.hh"

namespace lazarus
{
namespace schemas
{

//
// Structure for holding the request
// data for an object container.
//
class container_request
{
public:

    //
    // Constructor for the request interface.
    //
    container_request(
        const drogon::HttpRequestPtr& request);

    //
    // Move constructor for the request interface.
    //
    container_request(
        container_request&& other);

    //
    // Gets the object container name.
    // Returns an immutable reference to the name.
    //
    const std::string&
    get_name() const;

    //
    // Gets the object container request optype.
    //
    container_request_optype
    get_optype() const;


private:

    //
    // Object container name.
    //
    std::string name_;

    //
    // Expected object container name key tag in the client request.
    //
    static constexpr const char* name_key_tag = "object_container_name";

    //
    // Optype for the object container operation.
    //
    container_request_optype optype_;
};

} // namespace schemas.
} // namespace lazarus.