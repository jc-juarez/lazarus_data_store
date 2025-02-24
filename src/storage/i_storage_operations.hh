// ****************************************************
// Lazarus Data Store
// Storage
// 'i_storage_operations.hh'
// Author: jcjuarez
// Description:
//      Interface for accessing core storage operations. 
// ****************************************************

#pragma once

#include <string>

namespace lazarus
{
namespace storage
{

using byte = char;
using byte_stream = std::string;

//
// Storage core operations interface.
//
class i_storage_operations
{
public:

    //
    // Interface with destructor.
    //
    virtual ~i_storage_operations() = default;

    //
    // Inserts a single object into the data store.
    //
    virtual
    void
    insert_object(
        const char* object_id,
        const byte* object_data_buffer) = 0;

    //
    // Get an object from the data store.
    // Stores the object contents into the data stream if it exists.
    //
    virtual
    void
    get_object(
        const char* object_id,
        byte_stream& object_data_stream) = 0;
};

} // namespace storage.
} // namespace lazarus.