// ****************************************************
// Lazarus Data Store
// Status
// 'status.hh'
// Author: jcjuarez
// Description:
//      Status codes for error handling. 
// ****************************************************

#pragma once

#include "status_code.hh"
#include <drogon/drogon.h>

namespace lazarus
{
namespace status
{

//
// Macro for defining a constant expression status code instance.
//
#define status_code_definition(name, internal_status_code, http_status_code) \
    static inline constexpr status_code name{internal_status_code, http_status_code, #name}

//
// Operation succeeded.
//
status_code_definition(success, 0x00000000, drogon::HttpStatusCode::k200OK);

//
// Generic operation failed.
//
status_code_definition(fail, 0x80000001, drogon::HttpStatusCode::k500InternalServerError);

//
// Determines whether a given status is considered as failure.
//
inline static
bool
failed(
    const status_code& status_code)
{
    return static_cast<std::int32_t>(status_code) < 0;
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
// *****************************************
// Status codes for the lazarus data store.
// *****************************************
//

//
// Object container already exists.
//
status_code_definition(object_container_already_exists, 0x8'0000001, drogon::HttpStatusCode::k409Conflict);

//
// Invalid state for a given operation.
//
status_code_definition(invalid_operation, 0x8'0000002, drogon::HttpStatusCode::k400BadRequest);

//
// Storage engine fail to start.
//
status_code_definition(storage_engine_startup_failed, 0x8'0000003, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to insert an object into an object container.
//
status_code_definition(object_insertion_failed, 0x8'0000004, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to retrieve an object from an object container.
//
status_code_definition(object_retrieval_failed, 0x8'0000005, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to create an object container.
//
status_code_definition(object_container_creation_failed, 0x8'0000006, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to retrieve all the objects inside an object container.
//
status_code_definition(objects_retrieval_from_object_container_failed, 0x8'0000007, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to retrieve initial object containers from disk during startup.
//
status_code_definition(fetch_object_containers_from_disk_failed, 0x8'0000008, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to find the object containers internal metadata column family during startup.
//
status_code_definition(object_containers_internal_metadata_lookup_failed, 0x8'0000009, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to parse a byte stream into an object.
//
status_code_definition(parsing_failed, 0x8'0000010, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to find an storage engine reference for an object container.
//
status_code_definition(missing_storage_engine_reference, 0x8'0000011, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to close an object container storage engine reference.
//
status_code_definition(storage_engine_reference_close_failed, 0x8'0000012, drogon::HttpStatusCode::k500InternalServerError);

//
// Object container does not exist.
//
status_code_definition(object_container_not_exists, 0x8'0000013, drogon::HttpStatusCode::k404NotFound);

//
// Failed to serialize an object into a byte stream.
//
status_code_definition(serialization_failed, 0x8'0000014, drogon::HttpStatusCode::k500InternalServerError);

//
// Failed to remove an object from the storage engine.
//
status_code_definition(object_deletion_failed, 0x8'0000015, drogon::HttpStatusCode::k500InternalServerError);

//
// Specified object container is currently in deletion process.
//
status_code_definition(object_container_in_deletion_process, 0x8'0000016, drogon::HttpStatusCode::k409Conflict);

//
// Object container storage engine deletion failed.
//
status_code_definition(object_container_storage_engine_deletion_failed, 0x8'0000017, drogon::HttpStatusCode::k500InternalServerError);

//
// Object container name exceeds max size limit.
//
status_code_definition(object_container_name_exceeds_size_limit, 0x8'0000018, drogon::HttpStatusCode::k400BadRequest);

//
// Error for unreachable code blocks.
//
status_code_definition(unreachable, 0x8'0000019, drogon::HttpStatusCode::k500InternalServerError);

//
// Max number of object containers reached.
//
status_code_definition(max_number_object_containers_reached, 0x8'0000020, drogon::HttpStatusCode::k429TooManyRequests);

//
// Given object container name is empty.
//
status_code_definition(object_container_name_empty, 0x8'0000021, drogon::HttpStatusCode::k400BadRequest);

//
// Given object ID is empty.
//
status_code_definition(object_id_empty, 0x8'0000022, drogon::HttpStatusCode::k400BadRequest);

//
// Given object data stream is empty.
//
status_code_definition(object_data_empty, 0x8'0000023, drogon::HttpStatusCode::k400BadRequest);

//
// Object ID exceeds max size limit.
//
status_code_definition(object_id_exceeds_size_limit, 0x8'0000024, drogon::HttpStatusCode::k400BadRequest);

//
// Object data stream exceeds max size limit.
//
status_code_definition(object_data_exceeds_size_limit, 0x8'0000025, drogon::HttpStatusCode::k400BadRequest);

} // namespace status.
} // namespace lazarus.