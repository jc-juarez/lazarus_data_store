// ****************************************************
// Lazarus Data Store
// Storage
// 'data_store_accessor.cc'
// Author: jcjuarez
// Description:
//      Accessor core storage operations. 
// ****************************************************

#include "storage_engine.hh"
#include "data_store_accessor.hh"

namespace lazarus
{
namespace storage
{

data_store_accessor::data_store_accessor(
    std::shared_ptr<storage_engine> storage_engine_handle)
    : storage_engine_(std::move(storage_engine_handle))
{}

void
data_store_accessor::insert_object(
    const char* object_id,
    const byte* object_data_buffer)
{
    storage_engine_->insert_object(
        object_id,
        object_data_buffer);
}

} // namespace storage.
} // namespace lazarus.