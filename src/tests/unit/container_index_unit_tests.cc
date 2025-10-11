// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
// ****************************************************
// Lazarus Data Store
// Tests
// 'container_index_unit_tests.hh'
// Author: jcjuarez
// Description:
//      Unit tests for the container index component.
// ****************************************************

#include <gtest/gtest.h>
#include "../../core/status/status.hh"
#include "../mocks/mock_storage_engine.hh"
#include "core/storage/models/container.hh"
#include "core/storage/index/container_index.hh"

namespace lazarus
{
namespace tests
{

class container_index_unit_tests : public testing::Test
{
protected:

    void SetUp() override
    {
        //
        // Close container should always be successful.
        //
        EXPECT_CALL(
        *mock_storage_engine_, close_container_storage_engine_reference(testing::_))
        .WillRepeatedly(testing::Return(status::success));
    }

    //
    // Mock storage engine dependency.
    //
    std::shared_ptr<mock_storage_engine> mock_storage_engine_ = std::make_shared<mock_storage_engine>();

    //
    // Default number of container buckets
    //
    static constexpr std::uint16_t num_container_buckets_ = 8u;
};

TEST_F(
    container_index_unit_tests,
    insert_container_success)
{
    storage::container_index index{num_container_buckets_, mock_storage_engine_};
    storage::storage_engine_reference_handle* reference_handle{};
    schemas::container_persistent_interface container_metadata{};
    EXPECT_EQ(
        index.insert_container(reference_handle, container_metadata),
        status::success);
}

} // namespace tests.
} // namespace lazarus.