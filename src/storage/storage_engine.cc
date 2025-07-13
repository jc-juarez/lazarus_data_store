// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_engine.cc'
// Author: jcjuarez
// Description:
//      Core storage engine for handling IO operations. 
// ****************************************************

#include <thread>
#include "rocksdb/table.h"
#include <spdlog/spdlog.h>
#include "storage_engine.hh"
#include "../status/status.hh"

namespace lazarus
{
namespace storage
{

storage_engine::storage_engine(
    const storage_configuration& storage_configuration)
    : storage_configuration_{storage_configuration},
      core_database_{nullptr}
{}

status::status_code
storage_engine::start(
    const std::vector<std::string>& containers_names,
    std::unordered_map<std::string, storage_engine_reference_handle*>* storage_engine_references_mapping)
{
    std::vector<rocksdb::ColumnFamilyDescriptor> column_family_descriptors;
    
    //
    // Append all mappings for the core database initialization.
    //
    for (const auto& container_name : containers_names)
    {
        column_family_descriptors.emplace_back(
            container_name,
            rocksdb::ColumnFamilyOptions());
    }

    rocksdb::DB* database_handle;
    const rocksdb::Options options = get_engine_configurations();
    std::vector<storage_engine_reference_handle*> storage_engine_references;

    //
    // Start the engine. At this point the system will start
    // spinning up all internal resources for handling IO operations.
    //
    const rocksdb::Status status = rocksdb::DB::Open(
        options,
        storage_configuration_.core_database_path_,
        column_family_descriptors,
        &storage_engine_references,
        &database_handle);

    if (!status.ok())
    {
        spdlog::critical("An error occurred while trying to start the storage engine. "
            "NumberObjectContainersOnDisk={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            containers_names.size(),
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));
        
        return status::storage_engine_startup_failed;
    }

    //
    // At this point, the storage engine has been successfully started,
    // so assign the core database handle.
    //
    core_database_.reset(database_handle);

    //
    // Map the object container names to their respective column family references.
    //
    auto& mapping = *storage_engine_references_mapping;
    for (size_t index = 0; index < containers_names.size(); ++index)
    {
        mapping[containers_names[index]] = storage_engine_references[index];
    }

    return status::success;
}

status::status_code
storage_engine::insert_object(
    storage_engine_reference_handle* container_storage_engine_reference,
    const char* object_id,
    const byte_stream& object_data)
{
    const rocksdb::Status status = core_database_->Put(
        rocksdb::WriteOptions(),
        container_storage_engine_reference,
        object_id,
        object_data);

    if (!status.ok())
    {
        spdlog::error("Failed to insert object into the specified object container. "
            "ObjectId={}, "
            "ObjectContainerStorageEngineReference={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            object_id,
            static_cast<void*>(container_storage_engine_reference),
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));
        
        return status::object_insertion_failed;
    }

    return status::success;
}

status::status_code
storage_engine::get_object(
    storage_engine_reference_handle* container_storage_engine_reference,
    const char* object_id,
    byte_stream* object_data)
{
    const rocksdb::Status status = core_database_->Get(
        rocksdb::ReadOptions(),
        container_storage_engine_reference,
        object_id,
        object_data);

    if (!status.ok())
    {
        spdlog::error("Failed to retrieve object from the specified object container. "
            "ObjectId={}, "
            "ObjectContainerStorageEngineReference={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            object_id,
            static_cast<void*>(container_storage_engine_reference),
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));
        
        return status::object_retrieval_failed;
    }

    return status::success;
}

status::status_code
storage_engine::create_container(
    const char* container_name,
    storage_engine_reference_handle** container_storage_engine_reference)
{
    const rocksdb::Status status = core_database_->CreateColumnFamily(
        rocksdb::ColumnFamilyOptions(),
        container_name,
        container_storage_engine_reference);

    if (!status.ok())
    {
        //
        // On failure, nullify the reference.
        //
        container_storage_engine_reference = nullptr;

        spdlog::error("Failed to create the specified object container. "
            "ObjectContainerName={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            container_name,
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));

        return status::container_creation_failed;
    }

    return status::success;
}

status::status_code
storage_engine::get_all_objects_from_container(
    storage_engine_reference_handle* container_storage_engine_reference,
    std::unordered_map<std::string, byte_stream>* objects)
{
    rocksdb::ReadOptions read_options;
    std::unique_ptr<rocksdb::Iterator> it(core_database_->NewIterator(
        read_options,
        container_storage_engine_reference));

    //
    // Iterate over all objects in the object container
    // and append them to the list to return.
    //
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        std::string key = it->key().ToString();
        std::string value = it->value().ToString();
        objects->emplace(key, value);
    }

    //
    // If an error occurred, it will only be discovered after the storage
    // iteration, so error handling must be handled post iterator traversal.
    //
    const rocksdb::Status status = it->status();

    if (!status.ok())
    {
        spdlog::error("Failed to retrieve all objects inside the specified object container. "
            "ObjectContainerStorageEngineReference={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            static_cast<void*>(container_storage_engine_reference),
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));

        return status::objects_retrieval_from_container_failed;
    }

    return status::success;
}

status::status_code
storage_engine::fetch_containers_from_disk(
    std::vector<std::string>* containers_names)
{
    const rocksdb::Status status = rocksdb::DB::ListColumnFamilies(
        rocksdb::DBOptions(),
        storage_configuration_.core_database_path_,
        containers_names);

    //
    // rocksdb::Status::kPathNotFound is a valid error code
    // in case this is the first-time startup for the data store.
    //
    if (!status.ok() &&
        status.subcode() != rocksdb::Status::kPathNotFound)
    {
        spdlog::critical("Failed to retrieve initial object containers from disk. "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));

        return status::fetch_containers_from_disk_failed;
    }

    if (containers_names->empty())
    {
        //
        // In case there are no initial object containers, this is
        // a first-time or fresh startup for the data store, so the
        // default column family needs to be added to the core database initialization.
        //
        containers_names->push_back(rocksdb::kDefaultColumnFamilyName);
    }

    return status::success;
}

status::status_code
storage_engine::close_container_storage_engine_reference(
    storage_engine_reference_handle* container_storage_engine_reference)
{
    const rocksdb::Status status = core_database_->DestroyColumnFamilyHandle(
        container_storage_engine_reference);

    if (!status.ok())
    {
        spdlog::error("Failed to close storage engine reference. "
            "ObjectContainerStorageEngineReference={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            static_cast<void*>(container_storage_engine_reference),
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));

        return status::storage_engine_reference_close_failed;
    }

    return status::success;
}

status::status_code
storage_engine::remove_object(
    storage_engine_reference_handle* container_storage_engine_reference,
    const char* object_id)
{
    const rocksdb::Status status = core_database_->Delete(
        rocksdb::WriteOptions(),
        container_storage_engine_reference,
        object_id);

    if (!status.ok())
    {
        spdlog::error("Failed to remove object from the specified object container. "
            "ObjectContainerStorageEngineReference={}, "
            "ObjectId={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            static_cast<void*>(container_storage_engine_reference),
            object_id,
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));

        return status::object_deletion_failed;
    }

    return status::success;
}

status::status_code
storage_engine::remove_container(
    storage_engine_reference_handle* container_storage_engine_reference)
{
    const rocksdb::Status status = core_database_->DropColumnFamily(
        container_storage_engine_reference);

    if (!status.ok())
    {
        spdlog::error("Failed to remove object container from the storage engine. "
            "ObjectContainerStorageEngineReference={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            static_cast<void*>(container_storage_engine_reference),
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));

        return status::container_storage_engine_deletion_failed;
    }

    return status::success;
}

rocksdb::Options
storage_engine::get_engine_configurations() const
{
    rocksdb::Options options;
    options.create_if_missing = true;

    //
    // Increase the number of flushing threads to the
    // same as the number of logical cores on the system.
    //
    options.IncreaseParallelism(
        static_cast<std::int32_t>(std::thread::hardware_concurrency()));

    //
    // Optimize the compaction for
    // avoiding write stalls under heavy load.
    //
    options.OptimizeLevelStyleCompaction();

    //
    // The engine will start with a block cache with an LRU-backed policy.
    //
    rocksdb::BlockBasedTableOptions table_options;
    table_options.block_cache = rocksdb::NewLRUCache(
        storage_configuration_.storage_engine_block_cache_size_mib_ * 1024u * 1024u);
    options.table_factory.reset(NewBlockBasedTableFactory(table_options));

    return options;
}

} // namespace storage.
} // namespace lazarus.