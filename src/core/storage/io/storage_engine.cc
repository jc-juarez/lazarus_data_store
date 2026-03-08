// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'storage_engine.cc'
// Author: jcjuarez
// Description:
//      Core storage engine for handling IO operations. 
// ****************************************************

#include <spdlog/spdlog.h>
#include "storage_engine.hh"

namespace lazarus
{
namespace storage
{

storage_engine::storage_engine()
    : persistent_store_{nullptr} /* Intentional. Any non-initialized use should result in a segfault. */
{}

void
storage_engine::set_persistent_store(
    const std::uint16_t collocation_index,
    std::unique_ptr<rocksdb::DB> persistent_store)
{
    collocation_index_ = collocation_index;
    persistent_store_ = std::move(persistent_store);
}

status::status_code
storage_engine::insert_object(
    storage_engine_reference_handle* container_storage_engine_reference,
    const char* object_id,
    const byte_stream& object_data)
{
    const rocksdb::Status status = persistent_store_->Put(
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
    const rocksdb::Status status = persistent_store_->Get(
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
    const rocksdb::Status status = persistent_store_->CreateColumnFamily(
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
    std::unique_ptr<rocksdb::Iterator> it(persistent_store_->NewIterator(
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
storage_engine::close_container_storage_engine_reference(
    storage_engine_reference_handle* container_storage_engine_reference)
{
    const rocksdb::Status status = persistent_store_->DestroyColumnFamilyHandle(
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
    const rocksdb::Status status = persistent_store_->Delete(
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
    const rocksdb::Status status = persistent_store_->DropColumnFamily(
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

status::status_code
storage_engine::execute_objects_write_batch(
    storage_engine_write_batch& write_batch)
{
    const rocksdb::Status status = persistent_store_->Write(
        rocksdb::WriteOptions(),
        &write_batch);

    if (!status.ok())
    {
        spdlog::error("Failed to execute objects write batch operation. "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            static_cast<std::uint32_t>(status.code()),
            static_cast<std::uint32_t>(status.subcode()));

        return status::object_write_batch_failed;
    }

    return status::success;
}

} // namespace storage.
} // namespace lazarus.