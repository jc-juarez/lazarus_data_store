// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
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
#include "../status/status.hh"
#include "../common/aliases.hh"
#include "../common/aliases.hh"
#include <drogon/HttpController.h>
#include "storage_configuration.hh"
#include "../network/server/server.hh"
#include "../schemas/request-interfaces/container_request.hh"

namespace lazarus
{
namespace storage
{

class container_index;
class storage_engine_interface;
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
        std::shared_ptr<storage_engine_interface> storage_engine_handle,
        std::shared_ptr<container_index> container_index_handle,
        std::unique_ptr<container_operation_serializer> container_operation_serializer_handle);

    //
    // Populates the in-memory contents of the object container
    // index based on the references received from the storage engine start.
    //
    status::status_code
    populate_container_index(
        std::unordered_map<std::string, storage_engine_reference_handle*>* storage_engine_references_mapping);

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
    // Creates the root metadata column
    // families for the system if not present already.
    // On first-time startup, the data store will create them.
    //
    status::status_code
    create_internal_metadata_column_families(
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
    // Configurations for the storage subsystem.
    //
    const storage_configuration storage_configuration_;

    //
    // Handle for the storage engine.
    //
    std::shared_ptr<storage_engine_interface> storage_engine_;

    //
    // Handle for the object container index component.
    //
    std::shared_ptr<container_index> container_index_;

    //
    // Handle for the object container operation serializer component.
    //
    std::unique_ptr<container_operation_serializer> container_operation_serializer_;
};

} // namespace storage.
} // namespace lazarus.