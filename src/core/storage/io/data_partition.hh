// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// PandoraDB
// Storage
// 'data_partition.hh'
// Author: jcjuarez
// Description:
//      Data storage instance for IO access.
// ****************************************************

#pragma once

#include <memory>
#include <cstdint>
#include <rocksdb/db.h>
#include "../../status/status.hh"
#include "../../common/aliases.hh"
#include "../storage_configuration.hh"
#include "storage_engine_interface.hh"

namespace pandora
{
namespace storage
{

class data_partition
{
public:

    //
    // Constructor.
    // A valid storage engine instance must be provided.
    //
    data_partition(
        const std::string& partition_prefix,
        const std::uint16_t collocation_index,
        const storage_configuration& storage_configuration,
        std::unique_ptr<storage_engine_interface> storage_engine);

    //
    // Boots the data partition.
    // Requires a list of all object containers present on disk for the partition
    // and returns a list in respective order with all column family references.
    //
    status::status_code
    boot(
        const std::vector<std::string>& containers_names,
        std::unordered_map<std::string, storage_engine_reference*>& storage_engine_references_mapping);

    //
    // Fetches the existing object containers in the partition.
    // In case of a system crash, the underlying core engine ensures
    // that all operations are written into the WAL entries, thus pandora
    // only acknowledges object container references that were successfully
    // recorded into the storage engine WAL.
    // This should be invoked before booting the partition.
    //
    status::status_code
    fetch_containers_from_disk(
        std::vector<std::string>& containers_names);

    //
    // Provides access to the underlying storage engine.
    //
    storage_engine_interface&
    get_storage_engine();

    //
    // Gets the corresponding collocation index.
    //
    std::uint16_t
    get_collocation_index() const;

private:

    //
    // Generates the corresponding path for the data partition.
    //
    static
    std::string
    generate_partition_path(
        const std::string& prefix,
        const std::uint16_t collocation_index,
        const std::string& data_partitions_path);

    //
    // Generates the configurations to be used by the engine.
    //
    rocksdb::Options
    generate_engine_configurations() const;

    //
    // Storage engine for the data partition.
    // Access to the engine should be done while always
    // owning a valid reference to the data partition.
    //
    std::unique_ptr<storage::storage_engine_interface> storage_engine_;

    //
    // Configurations for the storage subsystem.
    //
    const storage_configuration storage_configuration_;

    //
    // Corresponding data collocation index.
    //
    const std::uint16_t collocation_index_;

    //
    // Corresponding data partition path for the storage engine internal key-value store.
    //
    const std::string partition_path_;
};

} // namespace storage.
} // namespace pandora.