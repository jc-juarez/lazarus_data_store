// ****************************************************
// Lazarus Data Store
// Storage
// 'object.hh'
// Author: jcjuarez
// Description:
//      Wrapper for holding an object associated data.
// ****************************************************

#pragma once

#include "../common/aliases.hh"

namespace lazarus::storage
{

class object
{
public:

    //
    // Constructor.
    //
    object(
        std::string&& object_id,
        byte_stream&& object_data,
        std::string&& container_name);

    //
    // Move constructor.
    //
    object(
        object&& other);

    //
    // Returns an immutable reference to the ID.
    //
    std::string&
    get_object_id();

    //
    // Returns an immutable reference to the data stream.
    //
    byte_stream&
    get_object_data();

    //
    // Returns an immutable reference to the associated container name.
    //
    std::string&
    get_container_name();

private:

    //
    // Owning buffer for the ID.
    //
    std::string object_id_;

    //
    // Owning buffer data stream.
    //
    byte_stream object_data_;

    //
    // Owning buffer for the associated container name.
    //
    std::string container_name_;
};

} // namespace lazarus::storage.