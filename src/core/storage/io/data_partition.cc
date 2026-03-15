// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'data_partition.cc'
// Author: jcjuarez
// Description:
//      Data storage partition instance for IO access.
// ****************************************************

#include "rocksdb/table.h"
#include "data_partition.hh"
#include "storage_engine.hh"

namespace pandora
{
namespace storage
{

data_partition::data_partition(
    const std::string& partition_prefix,
    const std::uint16_t collocation_index,
    const storage_configuration& storage_configuration,
    std::unique_ptr<storage_engine_interface> storage_engine)
    : collocation_index_{collocation_index},
      storage_engine_{std::move(storage_engine)},
      storage_configuration_{storage_configuration},
      partition_path_{generate_partition_path(
        partition_prefix,
        collocation_index,
        storage_configuration.kv_store_path_)}
{}

status::status_code
data_partition::boot(
    const std::vector<std::string>& containers_names,
    std::unordered_map<std::string, storage_engine_reference*>& storage_engine_references_mapping)
{
    std::vector<rocksdb::ColumnFamilyDescriptor> column_family_descriptors;

    //
    // Append all mappings for the core key-value store initialization.
    //
    for (const auto& container_name : containers_names)
    {
        column_family_descriptors.emplace_back(
            container_name,
            rocksdb::ColumnFamilyOptions());
    }

    if (!std::filesystem::exists(partition_path_))
    {
        TRACE_LOG(info, "Expected data partition not found for CollocationIndex={}. "
            "Creating directory for the partition at '{}'.",
            collocation_index_,
            partition_path_);

        std::filesystem::create_directories(partition_path_);
    }
    else
    {
        TRACE_LOG(info, "Existing data partition found for CollocationIndex={} "
            "at directory '{}'.",
            collocation_index_,
            partition_path_);
    }

    rocksdb::DB* persistent_store;
    const rocksdb::Options options = generate_engine_configurations();
    std::vector<storage_engine_reference*> storage_engine_references;

    //
    // Start the engine. At this point the system will start
    // spinning up all internal resources for handling IO operations.
    //
    const rocksdb::Status engine_status = rocksdb::DB::Open(
        options,
        partition_path_,
        column_family_descriptors,
        &storage_engine_references,
        &persistent_store);

    if (!engine_status.ok())
    {
        TRACE_LOG(critical, "An error occurred while trying to start the storage engine. "
            "NumberObjectContainersOnDisk={}, "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            containers_names.size(),
            static_cast<std::uint32_t>(engine_status.code()),
            static_cast<std::uint32_t>(engine_status.subcode()));

        return status::storage_engine_startup_failed;
    }

    //
    // Make sure none of the provided references from the engine are null.
    //
    for (storage_engine_reference* engine_reference : storage_engine_references)
    {
        if (engine_reference == nullptr)
        {
            return status::null_storage_engine_reference;
        }
    }

    //
    // All the engine references returned at this point must be
    // considered as approved for the storage engine from this partition
    // as they are mapping exactly to the internal state of the engine, so register them.
    //
    storage_engine_->register_approved_engine_references(
        storage_engine_references);

    //
    // At this point, the storage engine has been successfully started,
    // so assign the core persistent key-value store handle.
    //
    storage_engine_->set_persistent_store(
        collocation_index_,
        std::unique_ptr<rocksdb::DB>(persistent_store));

    //
    // Map the object container names to their respective column family references.
    //
    auto& mapping = storage_engine_references_mapping;
    for (size_t index = 0; index < containers_names.size(); ++index)
    {
        mapping[containers_names[index]] = storage_engine_references[index];
    }

    return status::success;
}

status::status_code
data_partition::fetch_containers_from_disk(
    std::vector<std::string>& containers_names)
{
    const rocksdb::Status engine_status = rocksdb::DB::ListColumnFamilies(
        rocksdb::DBOptions(),
        partition_path_,
        &containers_names);

    //
    // rocksdb::Status::kPathNotFound is a valid error code
    // in case this is the first-time startup for the data store.
    //
    if (!engine_status.ok() &&
        engine_status.subcode() != rocksdb::Status::kPathNotFound)
    {
        TRACE_LOG(critical, "Failed to retrieve initial object containers from disk. "
            "StorageEngineCode={}, "
            "StorageEngineSubCode={}.",
            static_cast<std::uint32_t>(engine_status.code()),
            static_cast<std::uint32_t>(engine_status.subcode()));

        return status::fetch_containers_from_disk_failed;
    }

    if (containers_names.empty())
    {
        //
        // In case there are no initial object containers, this is
        // a first-time or fresh startup for the data store, so the
        // default column family needs to be added to the core key-value store initialization.
        //
        containers_names.push_back(rocksdb::kDefaultColumnFamilyName);
    }

    return status::success;
}

storage_engine_interface&
data_partition::get_storage_engine()
{
    return *(storage_engine_.get());
}

std::uint16_t
data_partition::get_collocation_index() const
{
    return collocation_index_;
}

std::string
data_partition::generate_partition_path(
    const std::string& prefix,
    const std::uint16_t collocation_index,
    const std::string& data_partitions_path)
{
    return data_partitions_path + "/" + prefix + "-" + std::to_string(collocation_index);
}

rocksdb::Options
data_partition::generate_engine_configurations() const
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
} // namespace pandora.