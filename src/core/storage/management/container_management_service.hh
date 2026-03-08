// ****************************************************
// Copyright (c) 2025-Present Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Storage
// 'container_management_service.hh'
// Author: jcjuarez
// Description:
//      Management service for object container
//      operations.
// ****************************************************

#pragma once

#include <memory>
#include <optional>
#include <tbb/tbb.h>
#include "../../status/status.hh"
#include "../../common/aliases.hh"
#include "../../common/aliases.hh"
#include <drogon/HttpController.h>
#include "../storage_configuration.hh"
#include "../../network/server/server.hh"
#include "../index/container_reference_registry.hh"
#include "../models/container_partition_metadata.hh"
#include "../../schemas/request-interfaces/container_request.hh"

namespace lazarus
{
namespace storage
{

class data_partition;
class container_index;
class data_partition_provider;
class container_operation_serializer;

//
// Core storage access interface.
//
class container_management_service
{
public:

    //
    // Constructor data service.
    //
    container_management_service(
        const storage_configuration& storage_configuration,
        data_partition& container_metadata_partition,
        container_index& container_index_handle,
        std::unique_ptr<container_operation_serializer> container_operation_serializer_handle,
        data_partition_provider& data_partition_provider);

    //
    // Populates the in-memory contents of the object container
    // index based on the references received from the storage engine start.
    //
    status::status_code
    populate_container_index(
        std::unordered_map<std::string, storage_engine_reference_handle*>& container_metadata_partition_references,
        container_reference_registry& structured_partitions_registry);

    //
    // Orchestrates possible update operations to the object container index.
    // Given possible check-then-act race conditions upon object
    // container creation/deletion, these operations need to be serialized.
    // Handles callback response.
    //
    void
    orchestrate_serial_container_operation(
        schemas::container_request&& container_request,
        network::server_response_callback&& response_callback);

    //
    // Validates if an object container operation request.
    //
    status::status_code
    validate_container_operation_request(
        const schemas::container_request& container_request);

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
    // Validates if the given create request is valid.
    //
    status::status_code
    validate_container_create_request(
        const schemas::container_request& container_request);

    //
    // Validates if the given remove request is valid.
    //
    status::status_code
    validate_container_remove_request(
        const schemas::container_request& container_request);

    //
    // Indexes the container metadata partition containers.
    //
    status::status_code
    index_containers_from_container_metadata_partition(
        std::unordered_map<std::string, storage_engine_reference_handle*>& container_metadata_partition_references);

    //
    // Indexes the structured data partition containers known to the persistent container metadata.
    //
    status::status_code
    index_structured_partition_containers(
        container_reference_registry& structured_partitions_registry,
        std::unordered_map<std::string, byte_stream>& containers_present_on_metadata);

    //
    // Scans for orphaned containers and index them for garbage collection if found.
    //
    status::status_code
    scan_and_index_orphaned_containers(
        container_reference_registry& structured_partitions_registry,
        std::unordered_map<std::string, byte_stream>& containers_present_on_metadata);

    //
    // Converts a given list of ordered storage engine references by collocation index
    // into a list of container metadata instances.
    //
    std::vector<container_partition_metadata>
        convert_ordered_engine_references_to_container_instances(
        const std::vector<storage_engine_reference_handle*> storage_engine_references);

    //
    // Name for the default container for the container metadata data partitions.
    //
    static constexpr const char* k_default_container_name_metadata_partition = "metadata_default";

    //
    // Configurations for the storage subsystem.
    //
    const storage_configuration storage_configuration_;

    //
    // Handle for the container metadata partition.
    //
    data_partition& container_metadata_partition_;

    //
    // Handle for the object container index component.
    //
    container_index& container_index_;

    //
    // Handle for the object container operation serializer component.
    //
    std::unique_ptr<container_operation_serializer> container_operation_serializer_;

    //
    // Handle for the data partition provider component.
    //
    data_partition_provider& data_partition_provider_;
};

} // namespace storage.
} // namespace lazarus.