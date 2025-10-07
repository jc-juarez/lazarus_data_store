// ****************************************************
// Lazarus Data Store
// Tests
// 'container_bucket_unit_tests.hh'
// Author: jcjuarez
// Description:
//      Unit tests for the container bucket component.
// ****************************************************

#include <gtest/gtest.h>
#include "../../core/status/status.hh"
#include "../mocks/mock_storage_engine.hh"
#include "../../core/storage/container_bucket.hh"

namespace lazarus
{
namespace tests
{

class container_bucket_unit_tests : public testing::Test
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
};

TEST_F(
    container_bucket_unit_tests,
    insert_container_success)
{
    storage::container_bucket bucket{mock_storage_engine_};
    storage::storage_engine_reference_handle* reference_handle{};
    schemas::container_persistent_interface container_metadata{};
    EXPECT_EQ(
        bucket.insert_container(reference_handle, container_metadata),
        status::success);
}

TEST_F(
    container_bucket_unit_tests,
    insert_container_collision)
{
    storage::container_bucket bucket{mock_storage_engine_};
    storage::storage_engine_reference_handle* reference_handle{};
    const schemas::container_persistent_interface container_metadata1 =
        storage::container::create_container_persistent_metadata("Container");
    EXPECT_EQ(
        bucket.insert_container(reference_handle, container_metadata1),
        status::success);

    //
    // Append a new entry with the same name and expect a collision error.
    //
    const schemas::container_persistent_interface container_metadata2 =
        storage::container::create_container_persistent_metadata("Container");
    EXPECT_EQ(
        bucket.insert_container(reference_handle, container_metadata2),
        status::container_insertion_collision);
}

TEST_F(
    container_bucket_unit_tests,
    insert_container_different_names)
{
    storage::container_bucket bucket{mock_storage_engine_};
    storage::storage_engine_reference_handle* reference_handle{};
    const schemas::container_persistent_interface container_metadata1 =
        storage::container::create_container_persistent_metadata("Container1");
    EXPECT_EQ(
        bucket.insert_container(reference_handle, container_metadata1),
        status::success);
    const schemas::container_persistent_interface container_metadata2 =
        storage::container::create_container_persistent_metadata("Container2");
    EXPECT_EQ(
        bucket.insert_container(reference_handle, container_metadata2),
        status::success);
}

} // namespace tests.
} // namespace lazarus.