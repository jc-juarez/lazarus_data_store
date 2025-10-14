// ****************************************************
// Lazarus Data Store
// Status
// 'status.hh'
// Author: Auto-Generated
// Description:
//      Status codes for error handling.
//      Do not edit manually. Use
//      'scripts/generate_status_codes.py' to add
//      new status codes.
// ****************************************************

#pragma once

#include "status_code.hh"
#include <drogon/drogon.h>

namespace lazarus
{
namespace status
{

#define status_code_definition(name, internal, http) \
    static inline constexpr status_code name{internal, http, #name}

// Operation succeeded.
status_code_definition(
    success,
    0x00000000,
    static_cast<drogon::HttpStatusCode>(200));

// Generic operation failed.
status_code_definition(
    fail,
    0x80000001,
    static_cast<drogon::HttpStatusCode>(500));

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

// Object container already exists.
status_code_definition(
    container_already_exists,
    0x80000002,
    static_cast<drogon::HttpStatusCode>(409));

// Invalid state for a given operation.
status_code_definition(
    invalid_operation,
    0x80000003,
    static_cast<drogon::HttpStatusCode>(400));

// Storage engine failed to start.
status_code_definition(
    storage_engine_startup_failed,
    0x80000004,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to insert an object into an object container.
status_code_definition(
    object_insertion_failed,
    0x80000005,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to retrieve an object from an object container.
status_code_definition(
    object_retrieval_failed,
    0x80000006,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to create an object container.
status_code_definition(
    container_creation_failed,
    0x80000007,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to retrieve all the objects inside an object container.
status_code_definition(
    objects_retrieval_from_container_failed,
    0x80000008,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to retrieve initial object containers from disk during startup.
status_code_definition(
    fetch_containers_from_disk_failed,
    0x80000009,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to find the object containers internal metadata column family during startup.
status_code_definition(
    containers_internal_metadata_lookup_failed,
    0x8000000a,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to parse a byte stream into an object.
status_code_definition(
    parsing_failed,
    0x8000000b,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to find a storage engine reference for an object container.
status_code_definition(
    missing_storage_engine_reference,
    0x8000000c,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to close an object container storage engine reference.
status_code_definition(
    storage_engine_reference_close_failed,
    0x8000000d,
    static_cast<drogon::HttpStatusCode>(500));

// Object container does not exist.
status_code_definition(
    container_not_exists,
    0x8000000e,
    static_cast<drogon::HttpStatusCode>(404));

// Failed to serialize an object into a byte stream.
status_code_definition(
    serialization_failed,
    0x8000000f,
    static_cast<drogon::HttpStatusCode>(500));

// Failed to remove an object from the storage engine.
status_code_definition(
    object_deletion_failed,
    0x80000010,
    static_cast<drogon::HttpStatusCode>(500));

// Specified object container is currently in deletion process.
status_code_definition(
    container_in_deletion_process,
    0x80000011,
    static_cast<drogon::HttpStatusCode>(409));

// Object container storage engine deletion failed.
status_code_definition(
    container_storage_engine_deletion_failed,
    0x80000012,
    static_cast<drogon::HttpStatusCode>(500));

// Object container name exceeds max size limit.
status_code_definition(
    container_name_exceeds_size_limit,
    0x80000013,
    static_cast<drogon::HttpStatusCode>(400));

// Error for unreachable code blocks.
status_code_definition(
    unreachable,
    0x80000014,
    static_cast<drogon::HttpStatusCode>(500));

// Max number of object containers reached.
status_code_definition(
    max_number_containers_reached,
    0x80000015,
    static_cast<drogon::HttpStatusCode>(429));

// Given object container name is empty.
status_code_definition(
    container_name_empty,
    0x80000016,
    static_cast<drogon::HttpStatusCode>(400));

// Given object ID is empty.
status_code_definition(
    object_id_empty,
    0x80000017,
    static_cast<drogon::HttpStatusCode>(400));

// Given object data stream is empty.
status_code_definition(
    object_data_empty,
    0x80000018,
    static_cast<drogon::HttpStatusCode>(400));

// Object ID exceeds max size limit.
status_code_definition(
    object_id_exceeds_size_limit,
    0x80000019,
    static_cast<drogon::HttpStatusCode>(400));

// Object data stream exceeds max size limit.
status_code_definition(
    object_data_exceeds_size_limit,
    0x8000001a,
    static_cast<drogon::HttpStatusCode>(400));

// Container insertion collision.
status_code_definition(
    container_insertion_collision,
    0x8000001b,
    static_cast<drogon::HttpStatusCode>(500));

// Object data size exceeds limit for the frontline cache.
status_code_definition(
    object_data_size_exceeds_cache_limit,
    0x8000001c,
    static_cast<drogon::HttpStatusCode>(500));

// Objects write batch operation failed.
status_code_definition(
    object_write_batch_failed,
    0x8000001d,
    static_cast<drogon::HttpStatusCode>(500));

} // namespace status.
} // namespace lazarus.