// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_engine.cc'
// Author: jcjuarez
// Description:
//      Core storage engine for handling IO operations. 
// ****************************************************

#include <filesystem>
#include <spdlog/spdlog.h>
#include "storage_engine.hh"

namespace lazarus
{
namespace storage
{

storage_engine::storage_engine(
    const storage_engine_configuration& storage_engine_configuration)
    : storage_engine_configuration_{storage_engine_configuration},
      core_database_(nullptr)
{}

void
storage_engine::start(
    std::vector<std::string>& object_containers_names,
    std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*>* column_family_references_mapping)
{
    std::vector<rocksdb::ColumnFamilyDescriptor> column_family_descriptors;
    
    //
    // Append all mappings for the core database initialization.
    //
    for (const auto& object_container_name : object_containers_names)
    {
        column_family_descriptors.emplace_back(
            object_container_name,
            rocksdb::ColumnFamilyOptions());
    }

    //
    // Create the core database directory in case it does not exist.
    //
    std::filesystem::create_directories(storage_engine_configuration_.core_database_path_);

    rocksdb::DB* database_handle;
    rocksdb::Options options;
    options.create_if_missing = true;
    std::vector<rocksdb::ColumnFamilyHandle*> column_family_references;

    const rocksdb::Status status = rocksdb::DB::Open(
        options,
        storage_engine_configuration_.core_database_path_,
        column_family_descriptors,
        &column_family_references,
        &database_handle);

    if (!status.ok())
    {
        spdlog::error("An error occurred while trying to start the storage engine.");
        // return error. Again,m only allow rocksdb::Status::kPathNotFound.
    }

    //
    // At this point, the storage engine has been successfully started,
    // so assign the core database handle.
    //
    core_database_.reset(database_handle);

    //
    // Map the object container names to their respective column family references.
    //
    auto& mapping = *column_family_references_mapping;
    for (size_t index = 0; index < object_containers_names.size(); ++index)
    {
        mapping[object_containers_names[index]] = column_family_references[index];
    }

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
        // spdlog::error("");
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

    if (!status.ok())
    {
        // spdlog::error("");
    }
}

rocksdb::ColumnFamilyHandle*
storage_engine::create_object_container(
    const char* name)
{
    rocksdb::ColumnFamilyHandle* column_family_reference;

    const rocksdb::Status status = core_database_->CreateColumnFamily(
        rocksdb::ColumnFamilyOptions(),
        name,
        &column_family_reference);

    if (!status.ok())
    {
        //
        // Nullify the reference.
        //
        column_family_reference = nullptr;
        // spdlog::error("");
    }

    return column_family_reference;
}

std::unordered_map<std::string, byte_stream>
storage_engine::get_all_objects_from_object_container(
    rocksdb::ColumnFamilyHandle* data_store_reference)
{
    std::unordered_map<std::string, byte_stream> objects;
    rocksdb::ReadOptions read_options;
    std::unique_ptr<rocksdb::Iterator> it(core_database_->NewIterator(
        read_options,
        data_store_reference));

    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        std::string value = it->value().ToString();
        objects.emplace(key, value);
    }

    return objects;
}

void
storage_engine::fetch_object_containers_from_disk(
    std::vector<std::string>* object_containers)
{
    std::vector<std::string> containers;

    const rocksdb::Status status = rocksdb::DB::ListColumnFamilies(
        rocksdb::DBOptions(),
        storage_engine_configuration_.core_database_path_,
        object_containers);

    if (!status.ok() && 
        status.subcode() != rocksdb::Status::kPathNotFound)
    {
        // only accept rocksdb::Status::kPathNotFound, else fail.
        // spdlog::error("");
        // return error.
    }

    if (object_containers->empty())
    {
        //
        // In case there are no initial object containers, this is
        // a first-time or fresh startup for the data store, so the
        // default column family needs to be added to the core database initialization.
        //
        object_containers->push_back(rocksdb::kDefaultColumnFamilyName);
    }
}

} // namespace storage.
} // namespace lazarus.