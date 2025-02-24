// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_engine.cc'
// Author: jcjuarez
// Description:
//      Core storage engine for handling IO operations. 
// ****************************************************

#include "storage_engine.hh"

namespace lazarus
{
namespace storage
{

storage_engine::storage_engine()
{
    rocksdb::DB* database_handle;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "lazarus_ds", &database_handle);

    if (!status.ok())
    {
        throw std::runtime_error("An error occured while starting the core storage database.");
    }

    core_database_.reset(database_handle);
}

void
storage_engine::insert_object(
    const char* object_id,
    const byte* object_data_buffer)
{
    const rocksdb::Status status = core_database_->Put(
        rocksdb::WriteOptions(),
        object_id,
        object_data_buffer);

    if (!status.ok())
    {
        // std::cerr << "Put failed: " << status.ToString() << std::endl;
    }
}

void
storage_engine::get_object(
    const char* object_id,
    byte_stream& object_data_stream)
{
    const rocksdb::Status status = core_database_->Get(
        rocksdb::ReadOptions(),
        object_id,
        &object_data_stream);

    if (status.ok())
    {
        // std::cout << "Value for 'hello': " << value << std::endl;
    }
    else
    {
        // std::cerr << "Get failed: " << status.ToString() << std::endl;
    }
}

} // namespace storage.
} // namespace lazarus.