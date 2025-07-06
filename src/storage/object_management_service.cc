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
#include "../common/request_validations.hh"

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
    status::status_code status = validate_request_parameters(object_request);

    if (status::failed(status))
    {
        //
        // Parameters validations failed.
        //
        return status;
    }

    //
    // Only log the request parameters after
    // they have been validated to be well-formed.
    //
    switch (object_request.get_optype())
    {
        case schemas::object_request_optype::insert:
        case schemas::object_request_optype::get:
        case schemas::object_request_optype::remove:
        {
            //
            // Valid optype to be executed.
            //
            return status::success;
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
    status::status_code status = common::request_validations::validate_object_container_name(
        object_request.get_object_container_name(),
        storage_configuration_);

    if (status::failed(status))
    {
        //
        // Given object container name is invalid.
        // Not logging the parameters as to avoid potential
        // large-buffer attacks in case the parameters are too big.
        //
        spdlog::error("Object operation will be failed as the "
            "object container name is invalid. "
            "Optype={}, "
            "ObjectContainerNameSizeInBytes={}, "
            "ObjectContainerNameMaxSizeInBytes={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_container_name().size(),
            storage_configuration_.max_object_container_name_size_bytes_,
            status);

        return status;
    }

    status = common::request_validations::validate_object_id(
        object_request.get_object_id(),
        storage_configuration_);

    if (status::failed(status))
    {
        //
        // Given object ID is invalid.
        // Not logging the parameters as to avoid potential
        // large-buffer attacks in case the parameters are too big.
        //
        spdlog::error("Object operation will be failed as the "
            "object ID is invalid. "
            "Optype={}, "
            "ObjectIdSizeInBytes={}, "
            "ObjectIdMaxSizeInBytes={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_id().size(),
            storage_configuration_.max_object_id_size_bytes_,
            status);

        return status;
    }

    status = common::request_validations::validate_object_data(
        object_request.get_object_data(),
        storage_configuration_);

    if (status::failed(status))
    {
        //
        // Given object data stream is invalid.
        // Not logging the parameters as to avoid potential
        // large-buffer attacks in case the parameters are too big.
        //
        spdlog::error("Object operation will be failed as the "
            "object data stream is invalid. "
            "Optype={}, "
            "ObjectDataStreamSizeInBytes={}, "
            "ObjectDataStreamMaxSizeInBytes={}, "
            "Status={:#x}.",
            static_cast<std::uint8_t>(object_request.get_optype()),
            object_request.get_object_data().size(),
            storage_configuration_.max_object_data_size_bytes_,
            status);

        return status;
    }

    return status::success;
}

} // namespace storage.
} // namespace lazarus.