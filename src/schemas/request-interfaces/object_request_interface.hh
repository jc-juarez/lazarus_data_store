// ****************************************************
// Lazarus Data Store
// Schemas
// 'object_request_interface.hh'
// Author: jcjuarez
// Description:
//      Structure for defining an object
//      from a network request.
// ****************************************************

#pragma once

#include <string>
#include <cstdint>
#include "../../common/aliases.hh"
#include <drogon/HttpController.h>
#include "../request-optypes/object_request_optype.hh"

namespace lazarus
{
namespace schemas
{

//
// Structure for holding the request
// data for an object.
//
class object_request_interface
{
public:

    //
    // Constructor for the request interface.
    //
    object_request_interface(
        const drogon::HttpRequestPtr& request);

    //
    // Move constructor for the request interface.
    //
    object_request_interface(
        object_request_interface&& other);

    //
    // Gets the object container name.
    //
    const char*
    get_object_container_name() const;

    //
    // Gets the object ID.
    //
    const char*
    get_object_id() const;

    //
    // Gets the object data.
    // Returns a non-owning pointer to the byte stream.
    //
    const storage::byte*
    get_object_data() const;

    //
    // Gets the object request optype.
    //
    object_request_optype
    get_optype() const;

private:

    //
    // Object container name.
    //
    std::string object_container_name_;

    //
    // Object ID.
    //
    std::string object_id_;

    //
    // Object data.
    // Owning byte stream.
    //
    storage::byte_stream object_data_;

    //
    // Expected object container name key tag in the client request.
    //
    static constexpr const char* object_container_name_key_tag = "object_container_name";

    //
    // Expected object ID key tag in the client request.
    //
    static constexpr const char* object_id_key_tag = "object_id";

    //
    // Expected object data key tag in the client request.
    //
    static constexpr const char* object_data_key_tag = "object_data";

    //
    // Optype for the object operation.
    //
    object_request_optype optype_;
};

} // namespace schemas.
} // namespace lazarus.