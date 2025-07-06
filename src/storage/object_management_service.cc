// ****************************************************
// Lazarus Data Store
// Storage
// 'object_management_service.cc'
// Author: jcjuarez
// Description:
//      Management service for object operations.
// ****************************************************

#include <spdlog/spdlog.h>
#include "object_management_service.hh"

namespace lazarus
{
namespace storage
{

object_management_service::object_management_service(
    const storage_configuration& storage_configuration,
    std::shared_ptr<object_container_index> object_container_index)
    : storage_configuration_{storage_configuration},
      object_container_index_{std::move(object_container_index)}
{}

status::status_code
object_management_service::validate_object_operation_request(
    const schemas::object_request_interface& object_request)
{
    switch (object_request.get_optype())
    {
        case schemas::object_request_optype::insert:
        case schemas::object_request_optype::get:
        case schemas::object_request_optype::remove:
        {
            return validate_request_parameters(object_request);
            break;
        }
        default:
        {
            spdlog::error("Invalid optype received for object operation. "
                "Optype={}, "
                "ObjectId={}, "
                "ObjectContainerName={}.",
                static_cast<std::uint8_t>(object_request.get_optype()),
                object_request.get_object_id(),
                object_request.get_object_container_name());

            return status::invalid_operation;
            break;
        }
    }

    return status::unreachable;
}

status::status_code
object_management_service::validate_request_parameters(
    const schemas::object_request_interface& object_request)
{

}

} // namespace storage.
} // namespace lazarus.