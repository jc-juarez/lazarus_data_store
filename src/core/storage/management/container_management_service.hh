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
#include "../index/container_registry.hh"
#include "../models/container_instance.hh"
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