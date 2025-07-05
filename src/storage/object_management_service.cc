// ****************************************************
// Lazarus Data Store
// Storage
// 'object_management_service.cc'
// Author: jcjuarez
// Description:
//      Management service for object operations.
// ****************************************************

#include "object_management_service.hh"

namespace lazarus
{
namespace storage
{

object_management_service::object_management_service(
    std::shared_ptr<object_container_index> object_container_index)
    : object_container_index_{std::move(object_container_index)}
{}

status::status_code
object_management_service::validate_object_operation_request(
    const schemas::object_request_interface& object_request)
{
    return status::success;
}

} // namespace storage.
} // namespace lazarus.