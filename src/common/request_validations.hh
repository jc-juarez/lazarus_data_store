// ****************************************************
// Lazarus Data Store
// Schemas
// 'request_validations.hh'
// Author: jcjuarez
// Description:
//      Validations helper functions for requests.
// ****************************************************

#pragma once

#include <string>
#include "aliases.hh"
#include "../status/status.hh"
#include "../storage/storage_configuration.hh"

namespace lazarus::common::request_validations
{

//
// Validates whether an object container name is well-formed.
//
status::status_code
validate_object_container_name(
    const std::string& object_container_name,
    const storage::storage_configuration& storage_configuration);

//
// Validates whether an object ID is well-formed.
//
status::status_code
validate_object_id(
    const std::string& object_id,
    const storage::storage_configuration& storage_configuration);

//
// Validates whether an object data stream is well-formed.
//
status::status_code
validate_object_data(
    const storage::byte_stream& object_data,
    const storage::storage_configuration& storage_configuration);

} // namespace lazarus::common::request_validations.