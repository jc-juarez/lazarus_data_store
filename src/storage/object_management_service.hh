// ****************************************************
// Lazarus Data Store
// Storage
// 'object_management_service.hh'
// Author: jcjuarez
// Description:
//      Management service for object operations.
// ****************************************************

#pragma once

#include <memory>
#include "../status/status.hh"
#include "storage_configuration.hh"
#include "../schemas/request-interfaces/object_request_interface.hh"

namespace lazarus
{
namespace storage
{

class object_container;
class object_container_index;
class write_request_dispatcher;

//
// Core storage access interface.
//
class object_management_service
{
public:

    //
    // Constructor.
    //
    object_management_service(
        const storage_configuration& storage_configuration,
        std::shared_ptr<object_container_index> object_container_index,
        std::shared_ptr<write_request_dispatcher> write_request_dispatcher);

    //
    // Validates if an object operation request can be executed.
    //
    status::status_code
    validate_object_operation_request(
        const schemas::object_request_interface& object_request);

    //
    // Fetches the object container reference from the index.
    //
    std::shared_ptr<object_container>
    get_object_container_reference(
        const std::string& object_container_name);

    //
    // Enqueues the concurrent write request to the thread pool if successful.
    //
    status::status_code
    orchestrate_concurrent_write_request(
        schemas::object_request_interface&& object_request,
        std::shared_ptr<object_container> object_container,
        network::server_response_callback&& response_callback);

private:

    //
    // Validates the object request parameters.
    //
    status::status_code
    validate_request_parameters(
        const schemas::object_request_interface& object_request);

    //
    // Checks whether the given object operation optype is valid.
    //
    static
    bool
    is_object_operation_optype_valid(
        const schemas::object_request_optype optype);

    //
    // Checks if the given operation is write-oriented.
    //
    static
    bool
    is_object_request_write_io_operation(
        schemas::object_request_optype optype);

    //
    // Configurations for the storage subsystem.
    //
    const storage_configuration storage_configuration_;

    //
    // Handle for the object container index component.
    //
    std::shared_ptr<object_container_index> object_container_index_;

    //
    // Handle for the write request dispatcher component.
    //
    std::shared_ptr<write_request_dispatcher> write_request_dispatcher_;
};

} // namespace storage.
} // namespace lazarus.