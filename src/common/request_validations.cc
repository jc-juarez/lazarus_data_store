// ****************************************************
// Lazarus Data Store
// Schemas
// 'request_validations.cc'
// Author: jcjuarez
// Description:
//      Validations helper functions for requests.
// ****************************************************

#include "request_validations.hh"

namespace lazarus::common::request_validations
{

status::status_code
validate_object_container_name(
    const std::string& object_container_name,
    const storage::storage_configuration& storage_configuration)
{
    if (object_container_name.empty())
    {
        //
        // Given object container name is empty.
        //
        return status::object_container_name_empty;
    }

    if (object_container_name.size() >
        storage_configuration.max_object_container_name_size_bytes_)
    {
        //
        // Given object container name exceeds size limit.
        //
        return status::object_container_name_exceeds_size_limit;
    }

    return status::success;
}

status::status_code
validate_object_id(
    const std::string& object_id,
    const storage::storage_configuration& storage_configuration)
{
    if (object_id.empty())
    {
        //
        // Given object ID is empty.
        //
        return status::object_id_empty;
    }

    if (object_id.size() >
        storage_configuration.max_object_id_size_bytes_)
    {
        //
        // Given object ID exceeds size limit.
        //
        return status::object_id_exceeds_size_limit;
    }

    return status::success;
}

status::status_code
validate_object_data(
    const storage::byte_stream& object_data,
    const storage::storage_configuration& storage_configuration)
{
    if (object_data.empty())
    {
        //
        // Given object data stream is empty.
        //
        return status::object_data_empty;
    }

    if (object_data.size() >
        storage_configuration.max_object_data_size_bytes_)
    {
        //
        // Given object data stream exceeds size limit.
        //
        return status::object_data_exceeds_size_limit;
    }

    return status::success;
}

} // namespace lazarus::common::request_validations.