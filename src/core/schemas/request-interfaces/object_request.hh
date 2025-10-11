// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Schemas
// 'object_request.hh'
// Author: jcjuarez
// Description:
//      Structure for defining an object
//      from a network request.
// ****************************************************

#pragma once

#include <string>
#include <cstdint>
#include <drogon/HttpController.h>
#include "../../common/aliases.hh"
#include "../../storage/models/object.hh"
#include "../request-optypes/object_request_optype.hh"

namespace lazarus
{
namespace schemas
{

//
// Structure for holding the request
// data for an object.
//
class object_request
{
public:

    //
    // Constructor for the request interface.
    //
    object_request(
        const drogon::HttpRequestPtr& request);

    //
    // Move constructor for the request interface.
    //
    object_request(
        object_request&& other);

    //
    // Gets the object container name.
    // Returns an immutable reference to the container name.
    //
    const std::string&
    get_container_name() const;

    //
    // Gets the object ID.
    // Returns an immutable reference to the ID.
    //
    const std::string&
    get_object_id() const;

    //
    // Gets the object data.
    // Returns an immutable reference to the byte stream.
    //
    const storage::byte_stream&
    get_object_data() const;

    //
    // Gets the object container name.
    // Returns a mutable reference to the container name.
    //
    std::string&
    get_container_name_mutable();

    //
    // Gets the object ID.
    // Returns a mutable reference to the ID.
    //
    std::string&
    get_object_id_mutable();

    //
    // Gets the object data.
    // Returns a mutable reference to the byte stream.
    //
    storage::byte_stream&
    get_object_data_mutable();

    //
    // Gets the object request optype.
    //
    object_request_optype
    get_optype() const;

    //
    // Expected object container name key tag in the client request.
    //
    static constexpr const char* container_name_key_tag = "container_name";

    //
    // Expected object ID key tag in the client request.
    //
    static constexpr const char* object_id_key_tag = "object_id";

    //
    // Expected object data key tag in the client request.
    //
    static constexpr const char* object_data_key_tag = "object_data";

private:

    //
    // Owning object data contents.
    //
    std::unique_ptr<storage::object> object_;

    //
    // Optype for the object operation.
    //
    object_request_optype optype_;
};

} // namespace schemas.
} // namespace lazarus.