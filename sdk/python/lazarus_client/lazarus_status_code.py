# ****************************************************
# Lazarus Data Store
# Status
# 'lazarus_status_code.py'
# Author: Auto-Generated
# Description:
#      Status codes for error handling.
#      Do not edit manually. Use
#      'scripts/generate_status_codes.py' to add
#      new status codes.
# ****************************************************

from enum import Enum

class LazarusStatusCode(Enum):

    # Operation succeeded.
    success = 0x00000000

    # Generic operation failed.
    fail = 0x80000001

    # Object container already exists.
    container_already_exists = 0x80000002

    # Invalid state for a given operation.
    invalid_operation = 0x80000003

    # Storage engine failed to start.
    storage_engine_startup_failed = 0x80000004

    # Failed to insert an object into an object container.
    object_insertion_failed = 0x80000005

    # Failed to retrieve an object from an object container.
    object_retrieval_failed = 0x80000006

    # Failed to create an object container.
    container_creation_failed = 0x80000007

    # Failed to retrieve all the objects inside an object container.
    objects_retrieval_from_container_failed = 0x80000008

    # Failed to retrieve initial object containers from disk during startup.
    fetch_containers_from_disk_failed = 0x80000009

    # Failed to find the object containers internal metadata column family during startup.
    containers_internal_metadata_lookup_failed = 0x8000000a

    # Failed to parse a byte stream into an object.
    parsing_failed = 0x8000000b

    # Failed to find a storage engine reference for an object container.
    missing_storage_engine_reference = 0x8000000c

    # Failed to close an object container storage engine reference.
    storage_engine_reference_close_failed = 0x8000000d

    # Object container does not exist.
    container_not_exists = 0x8000000e

    # Failed to serialize an object into a byte stream.
    serialization_failed = 0x8000000f

    # Failed to remove an object from the storage engine.
    object_deletion_failed = 0x80000010

    # Specified object container is currently in deletion process.
    container_in_deletion_process = 0x80000011

    # Object container storage engine deletion failed.
    container_storage_engine_deletion_failed = 0x80000012

    # Object container name exceeds max size limit.
    container_name_exceeds_size_limit = 0x80000013

    # Error for unreachable code blocks.
    unreachable = 0x80000014

    # Max number of object containers reached.
    max_number_containers_reached = 0x80000015

    # Given object container name is empty.
    container_name_empty = 0x80000016

    # Given object ID is empty.
    object_id_empty = 0x80000017

    # Given object data stream is empty.
    object_data_empty = 0x80000018

    # Object ID exceeds max size limit.
    object_id_exceeds_size_limit = 0x80000019

    # Object data stream exceeds max size limit.
    object_data_exceeds_size_limit = 0x8000001a

    # Container insertion collision.
    container_insertion_collision = 0x8000001b

    # Object data size exceeds limit for the frontline cache.
    object_data_size_exceeds_cache_limit = 0x8000001c

    # Objects write batch operation failed.
    object_write_batch_failed = 0x8000001d

    @classmethod
    def from_code(cls, code: int):
        for s in cls:
            if s.value == code:
                return s
        return None