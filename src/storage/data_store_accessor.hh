// ****************************************************
// Lazarus Data Store
// Storage
// 'data_store_accessor.hh'
// Author: jcjuarez
// Description:
//      Accessor core storage operations. 
// ****************************************************

#pragma once

#include <memory>
#include "i_storage_operations.hh"

namespace lazarus
{
namespace storage
{

using byte = char;

class storage_engine;

//
// Core storage access interface.
//
class data_store_accessor : i_storage_operations
{
public:

    //
    // Constructor data accessor.
    //
    data_store_accessor(
        std::shared_ptr<storage_engine> storage_engine_handle);

    //
    // Inserts a single object into the data store.
    //
    void
    insert_object(
        const char* object_id,
        const byte* object_data_buffer);

private:

    //
    // Handle for the storage enine.
    //
    std::shared_ptr<storage_engine> storage_engine_;
};

} // namespace storage.
} // namespace lazarus.