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
#include "../schemas/request-interfaces/object_request_interface.hh"

namespace lazarus
{
namespace storage
{

class object_container_index;

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
        std::shared_ptr<object_container_index> object_container_index);

    //
    // Validates if an object operation request can be executed.
    //
    status::status_code
    validate_object_operation_request(
        const schemas::object_request_interface& object_request);

private:

    //
    // Handle for the object container index component.
    //
    std::shared_ptr<object_container_index> object_container_index_;
};

} // namespace storage.
} // namespace lazarus.