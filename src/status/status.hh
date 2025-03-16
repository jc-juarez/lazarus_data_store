// ****************************************************
// Lazarus Data Store
// Status
// 'status.hh'
// Author: jcjuarez
// Description:
//      Status codes for error handling. 
// ****************************************************

#pragma once

#include <cstdint>

namespace lazarus
{
namespace status
{

//
// Status code type alias.
//
using status_code = std::uint32_t;

#define status_code_definition(status_code_name, status_code_value) static constexpr status_code status_code_name = status_code_value

//
// Operation succeeded.
//
status_code_definition(success, 0x0'0000000);

//
// Generic operation failed.
//
status_code_definition(fail, 0x8'0000000);

//
// Determines whether a given status is considered as failure.
//
inline static
bool
failed(
    const status_code status_code)
{
    return static_cast<int>(status_code) < 0;
}

//
// Determines whether a given status is considered as success.
//
inline static
bool
succeeded(
    const status_code& status_code)
{
    return status_code == success;
}

//
// Determines whether a given status is the same as another status.
//
inline static
bool
is_same(
    const status_code& status_code_left,
    const status_code& status_code_right)
{
    return status_code_left == status_code_right;
}

//
// Determines whether a given status is not the same as another status.
//
inline static
bool
is_not_same(
    const status_code& status_code_left,
    const status_code& status_code_right)
{
    return status_code_left != status_code_right;
}

//
// *****************************************
// Status codes for the lazarus data store.
// *****************************************
//

//
// Object container already exists.
//
status_code_definition(object_container_already_exists, 0x8'0000001);

//
// Invalid state for a given operation.
//
status_code_definition(invalid_operation, 0x8'0000002);

//
// Storage engine fail to start.
//
status_code_definition(storage_engine_startup_failed, 0x8'0000003);

//
// Failed to insert an object into an object container.
//
status_code_definition(object_insertion_failed, 0x8'0000004);

//
// Failed to retrieve an object from an object container.
//
status_code_definition(object_retrieval_failed, 0x8'0000005);

//
// Failed to create an object container.
//
status_code_definition(object_container_creation_failed, 0x8'0000006);

//
// Failed to retrieve all the objects inside an object container.
//
status_code_definition(objects_retrieval_from_object_container_failed, 0x8'0000007);

//
// Failed to retrieve initial object containers from disk during startup.
//
status_code_definition(fetch_object_containers_from_disk_failed, 0x8'0000008);

//
// Failed to find the object containers internal metadata column family during startup.
//
status_code_definition(object_containers_internal_metadata_lookup_failed, 0x8'0000009);

//
// Failed to parse a byte stream into an object.
//
status_code_definition(parsing_failed, 0x8'0000010);

//
// Failed to find an storage engine reference for an object container.
//
status_code_definition(missing_storage_engine_reference, 0x8'0000011);

//
// Failed to close an object container storage engine reference.
//
status_code_definition(storage_engine_reference_close_failed, 0x8'0000012);

//
// Object container does not exist.
//
status_code_definition(object_container_not_exists, 0x8'0000013);

//
// Failed to serialize an object into a byte stream.
//
status_code_definition(serialization_failed, 0x8'0000014);

//
// Failed to remove an object from the storage engine.
//
status_code_definition(object_deletion_failed, 0x8'0000015);

//
// Specified object container is currently in deletion process.
//
status_code_definition(object_container_in_deletion_process, 0x8'0000016);

} // namespace status.
} // namespace lazarus.