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
    if (!fence_engine_reference(container_storage_engine_reference))
    {
        status::status_code status = status::storage_engine_reference_not_approved;
        TRACE_LOG(error, "Insert object operation is invalid since "
            "engine reference is not approved for the storage engine. "
            "ObjectId={}, "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "Status={:#x}.",
            object_id,
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            status);

        return status;
    }

    const rocksdb::Status engine_status = persistent_store_->Put(
        rocksdb::WriteOptions(),
        container_storage_engine_reference,
        object_id,
        object_data);

    if (!engine_status.ok())
    {
        status::status_code status = status::object_insertion_failed;
        TRACE_LOG(error, "Failed to insert object into the specified object container. "
            "ObjectId={}, "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}, "
            "Status={:#x}.",
            object_id,
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            static_cast<std::uint32_t>(engine_status.code()),
            static_cast<std::uint32_t>(engine_status.subcode()),
            status);
        
        return status;
    }

    return status::success;
}

status::status_code
storage_engine::get_object(
    storage_engine_reference_handle* container_storage_engine_reference,
    const char* object_id,
    byte_stream* object_data)
{
    if (!fence_engine_reference(container_storage_engine_reference))
    {
        status::status_code status = status::storage_engine_reference_not_approved;
        TRACE_LOG(error, "Get object operation is invalid since "
            "engine reference is not approved for the storage engine. "
            "ObjectId={}, "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "Status={:#x}.",
            object_id,
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            status);

        return status;
    }

    const rocksdb::Status engine_status = persistent_store_->Get(
        rocksdb::ReadOptions(),
        container_storage_engine_reference,
        object_id,
        object_data);

    if (!engine_status.ok())
    {
        status::status_code status = status::object_retrieval_failed;
        TRACE_LOG(error, "Failed to retrieve object from the specified object container. "
            "ObjectId={}, "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}, "
            "Status={:#x}.",
            object_id,
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            static_cast<std::uint32_t>(engine_status.code()),
            static_cast<std::uint32_t>(engine_status.subcode()),
            status);
        
        return status;
    }

    return status::success;
}

status::status_code
storage_engine::create_container(
    const char* container_name,
    storage_engine_reference_handle** container_storage_engine_reference)
{
    const rocksdb::Status engine_status = persistent_store_->CreateColumnFamily(
        rocksdb::ColumnFamilyOptions(),
        container_name,
        container_storage_engine_reference);

    if (!engine_status.ok())
    {
        //
        // On failure, nullify the reference.
        //
        container_storage_engine_reference = nullptr;

        status::status_code status = status::container_creation_failed;
        TRACE_LOG(error, "Failed to create the specified object container. "
            "ContainerName={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}, "
            "CollocationIndex={}, "
            "Status={:#x}.",
            container_name,
            static_cast<std::uint32_t>(engine_status.code()),
            static_cast<std::uint32_t>(engine_status.subcode()),
            collocation_index_,
            status);

        return status;
    }

    return status::success;
}

status::status_code
storage_engine::get_all_objects_from_container(
    storage_engine_reference_handle* container_storage_engine_reference,
    std::unordered_map<std::string, byte_stream>* objects)
{
    if (!fence_engine_reference(container_storage_engine_reference))
    {
        status::status_code status = status::storage_engine_reference_not_approved;
        TRACE_LOG(error, "Get all objects operation is invalid since "
            "engine reference is not approved for the storage engine. "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "Status={:#x}.",
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            status);

        return status;
    }

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
    const rocksdb::Status engine_status = it->status();

    if (!engine_status.ok())
    {
        status::status_code status = status::objects_retrieval_from_container_failed;
        TRACE_LOG(error, "Failed to retrieve all objects inside the specified object container. "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}, "
            "Status={:#x}.",
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            static_cast<std::uint32_t>(engine_status.code()),
            static_cast<std::uint32_t>(engine_status.subcode()),
            status);

        return status;
    }

    return status::success;
}

status::status_code
storage_engine::close_container_storage_engine_reference(
    storage_engine_reference_handle* container_storage_engine_reference)
{
    if (!fence_engine_reference(container_storage_engine_reference))
    {
        status::status_code status = status::storage_engine_reference_not_approved;
        TRACE_LOG(error, "Close engine reference operation is invalid since "
            "engine reference is not approved for the storage engine. "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "Status={:#x}.",
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            status);

        return status;
    }

    const rocksdb::Status engine_status = persistent_store_->DestroyColumnFamilyHandle(
        container_storage_engine_reference);

    if (!engine_status.ok())
    {
        status::status_code status = status::storage_engine_reference_close_failed;
        TRACE_LOG(error, "Failed to close storage engine reference. "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}, "
            "Status={:#x}.",
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            static_cast<std::uint32_t>(engine_status.code()),
            static_cast<std::uint32_t>(engine_status.subcode()),
            status);

        return status;
    }

    //
    // At this point, we are now safe to delete the approved reference for
    // fencing purposes.
    //
    approved_references_.erase(container_storage_engine_reference);

    return status::success;
}

status::status_code
storage_engine::remove_object(
    storage_engine_reference_handle* container_storage_engine_reference,
    const char* object_id)
{
    if (!fence_engine_reference(container_storage_engine_reference))
    {
        status::status_code status = status::storage_engine_reference_not_approved;
        TRACE_LOG(error, "Remove object operation is invalid since "
            "engine reference is not approved for the storage engine. "
            "ObjectId={}, "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "Status={:#x}.",
            object_id,
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            status);

        return status;
    }

    const rocksdb::Status engine_status = persistent_store_->Delete(
        rocksdb::WriteOptions(),
        container_storage_engine_reference,
        object_id);

    if (!engine_status.ok())
    {
        status::status_code status = status::object_deletion_failed;
        TRACE_LOG(error, "Failed to remove object from the specified object container. "
            "ObjectId={}, "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}, "
            "Status={:#x}.",
            object_id,
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            static_cast<std::uint32_t>(engine_status.code()),
            static_cast<std::uint32_t>(engine_status.subcode()),
            status);

        return status;
    }

    return status::success;
}

status::status_code
storage_engine::remove_container(
    storage_engine_reference_handle* container_storage_engine_reference)
{
    if (!fence_engine_reference(container_storage_engine_reference))
    {
        status::status_code status = status::storage_engine_reference_not_approved;
        TRACE_LOG(error, "Remove container operation is invalid since "
            "engine reference is not approved for the storage engine. "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "Status={:#x}.",
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            status);

        return status;
    }

    const rocksdb::Status engine_status = persistent_store_->DropColumnFamily(
        container_storage_engine_reference);

    if (!engine_status.ok())
    {
        status::status_code status = status::container_storage_engine_deletion_failed;
        TRACE_LOG(error, "Failed to remove object container from the storage engine. "
            "EngineReference={}, "
            "EngineReferenceID={}, "
            "ContainerName={}, "
            "CollocationIndex={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}, "
            "Status={:#x}.",
            static_cast<void*>(container_storage_engine_reference),
            container_storage_engine_reference->GetID(),
            container_storage_engine_reference->GetName(),
            collocation_index_,
            static_cast<std::uint32_t>(engine_status.code()),
            static_cast<std::uint32_t>(engine_status.subcode()),
            status);

        return status;
    }

    return status::success;
}

void
storage_engine::register_approved_engine_references(
    const std::vector<storage_engine_reference_handle*> engine_references)
{
    for (const auto& engine_reference : engine_references)
    {
        approved_references_.insert(
            engine_reference,
            std::monostate{});
    }
}

bool
storage_engine::fence_engine_reference(
    storage_engine_reference_handle* engine_reference)
{
    return engine_reference != nullptr &&
           approved_references_.exists(engine_reference);
}

} // namespace storage.
} // namespace lazarus.