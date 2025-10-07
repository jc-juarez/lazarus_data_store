// ****************************************************
// Lazarus Data Store
// Tests
// 'mock_storage_engine.hh'
// Author: jcjuarez
// Description:
//      Mock storage engine for tests.
// ****************************************************

#pragma once

#include <gmock/gmock.h>
#include "../../storage/storage_engine_interface.hh"

namespace lazarus
{
namespace tests
{

class mock_storage_engine : public storage::storage_engine_interface
{
public:

    MOCK_METHOD(
        lazarus::status::status_code,
        start,
        ((const std::vector<std::string>&),
        (std::unordered_map<std::string, storage::storage_engine_reference_handle*>*)),
        (override));

    MOCK_METHOD(
        status::status_code,
        insert_object,
        (storage::storage_engine_reference_handle*,
        const char*,
        const storage::byte_stream&),
        (override));

    MOCK_METHOD(
        status::status_code,
        get_object,
        (storage::storage_engine_reference_handle*,
        const char*,
        storage::byte_stream*),
        (override));

    MOCK_METHOD(
        status::status_code,
        create_container,
        (const char*,
        storage::storage_engine_reference_handle**),
        (override));

    MOCK_METHOD(
        status::status_code,
        get_all_objects_from_container,
        ((storage::storage_engine_reference_handle*),
        (std::unordered_map<std::string, storage::byte_stream>*)),
        (override));

    MOCK_METHOD(
        status::status_code,
        fetch_containers_from_disk,
        (std::vector<std::string>*),
        (override));

    MOCK_METHOD(
        status::status_code,
        close_container_storage_engine_reference,
        (storage::storage_engine_reference_handle*),
        (override));

    MOCK_METHOD(
        status::status_code,
        remove_object,
        (storage::storage_engine_reference_handle*,
        const char*),
        (override));

    MOCK_METHOD(
        status::status_code,
        remove_container,
        (storage::storage_engine_reference_handle * ),
        (override));
};

} // namespace tests.
} // namespace lazarus.