// ****************************************************
// Lazarus Data Store
// Storage
// 'object_container_management_service.hh'
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
#include "../schemas/request-interfaces/object_container_request_interface.hh"

namespace lazarus
{
namespace storage
{

class storage_engine;
class object_container_index;
class object_container_operation_serializer;

//
// Core storage access interface.
//
class object_container_management_service
{
public:

    //
    // Constructor data service.
    //
    object_container_management_service(
        const storage_configuration& storage_configuration,
        std::shared_ptr<storage_engine> storage_engine_handle,
        std::shared_ptr<object_container_index> object_container_index_handle,
        std::unique_ptr<object_container_operation_serializer> object_container_operation_serializer_handle);

    //
    // Populates the in-memory contents of the object container
    // index based on the references received from the storage engine start.
    //
    status::status_code
    populate_object_container_index(
        std::unordered_map<std::string, storage_engine_reference_handle*>* storage_engine_references_mapping);

    //
    // Orchestrates possible update operations to the object container index.
    // Given possible check-then-act race conditions upon object
    // container creation/deletion, these operations need to be serialized.
    // Handles callback response.
    //
    void
    orchestrate_serial_object_container_operation(
        schemas::object_container_request_interface&& object_container_request,
        network::server_response_callback&& response_callback);

    //
    // Validates if an object container operation request.
    //
    status::status_code
    validate_object_container_operation_request(
        const schemas::object_container_request_interface& object_container_request);

private:

    //
    // Creates the root metadata column
    // families for the system if not present already.
    // On first-time startup, the data store will create them.
    //
    status::status_code
    create_internal_metadata_column_families(
        std::unordered_map<std::string, storage_engine_reference_handle*>* storage_engine_references_mapping);

    status::status_code
    validate_object_container_create_request(
        const schemas::object_container_request_interface& object_container_request);

    status::status_code
    validate_object_container_remove_request(
        const schemas::object_container_request_interface& object_container_request);

    //
    // Configurations for the storage subsystem.
    //
    const storage_configuration storage_configuration_;

    //
    // Handle for the storage engine.
    //
    std::shared_ptr<storage_engine> storage_engine_;

    //
    // Handle for the object container index component.
    //
    std::shared_ptr<object_container_index> object_container_index_;

    //
    // Handle for the object container operation serializer component.
    //
    std::unique_ptr<object_container_operation_serializer> object_container_operation_serializer_;
};

} // namespace storage.
} // namespace lazarus.