// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_loader.hh'
// Author: jcjuarez
// Description:
//      Orchestrates the logic for loading containers
//      found on the filesystem into the index.
// ****************************************************

#pragma once

#include "../../status/status.hh"
#include "../../common/aliases.hh"
#include "../models/container_instance.hh"

namespace lazarus
{
namespace storage
{

class data_partition;
class container_index;
class container_registry;
class data_partition_provider;

class container_loader
{
public:

    //
    // Constructor.
    //
    container_loader(
        data_partition& metadata_partition,
        container_index& container_index,
        data_partition_provider& data_partition_provider);

    //
    // Loads the provided containers into the container index
    // based on the state of the containers metadata and system state.
    //
    status::status_code
    load_container_index(
        std::unordered_map<std::string, storage_engine_reference_handle*>& metadata_partition_references,
        container_registry& structured_partitions_registry);

    //
    // Name for the default container in the metadata partition.
    //
    static constexpr const char* k_default_container_name_metadata_partition = "metadata_default";

    //
    // Name for the containers container in the metadata partition.
    //
    static constexpr const char* k_containers_container_name_metadata_partition = "metadata_containers";

private:

    //
    // Creates the root container metadata column
    // families for the system if not present already.
    // On first-time startup, the data store will create them.
    //
    status::status_code
    create_container_metadata_column_family(
        std::unordered_map<std::string, storage_engine_reference_handle*>* storage_engine_references_mapping);

    //
    // Indexes the container metadata partition containers.
    //
    status::status_code
    index_containers_from_metadata_partition(
        std::unordered_map<std::string, storage_engine_reference_handle*>& metadata_partition_references);

    //
    // Indexes the structured data partition containers known to the persistent container metadata.
    //
    status::status_code
    index_structured_partition_containers(
        container_registry& structured_partitions_registry,
        std::unordered_map<std::string, byte_stream>& containers_present_on_metadata);

    //
    // Scans for orphaned containers and index them for garbage collection if found.
    //
    status::status_code
    scan_and_index_orphaned_containers(
        container_registry& structured_partitions_registry,
        std::unordered_map<std::string, byte_stream>& containers_present_on_metadata);

    //
    // Converts a given list of ordered storage engine references by collocation index
    // into a list of container metadata instances.
    //
    std::vector<container_instance>
    convert_ordered_engine_references_to_container_instances(
        const std::vector<storage_engine_reference_handle*> storage_engine_references);

    //
    // Max number of expected containers for the metadata partition.
    // This should correspond to the default container and the containers metadata container.
    //
    static constexpr std::uint8_t k_max_metadata_partition_containers = 2u;

    //
    // Handle for the metadata partition.
    //
    data_partition& metadata_partition_;

    //
    // Handle for the index component.
    //
    container_index& container_index_;

    //
    // Handle for the data partition provider.
    //
    data_partition_provider& data_partition_provider_;
};

} // namespace storage.
} // namespace lazarus.