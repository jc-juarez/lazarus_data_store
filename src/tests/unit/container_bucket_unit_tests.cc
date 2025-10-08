// ****************************************************
// Copyright (c) 2025 Juan Carlos Juarez Garcia
// Licensed under the Business Source License 1.1
// See the LICENSE file in the
// project root for license terms.
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
#include "../../core/storage/container.hh"
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

TEST_F(
    container_bucket_unit_tests,
    get_container_present)
{
    storage::container_bucket bucket{mock_storage_engine_};
    storage::storage_engine_reference_handle* reference_handle{};
    const schemas::container_persistent_interface container_metadata =
        storage::container::create_container_persistent_metadata("Container");
    EXPECT_EQ(
        bucket.insert_container(reference_handle, container_metadata),
        status::success);
    std::shared_ptr<storage::container> container = bucket.get_container("Container");
    EXPECT_NE(
        container,
        nullptr);
}

TEST_F(
    container_bucket_unit_tests,
    get_container_not_present)
{
    storage::container_bucket bucket{mock_storage_engine_};
    EXPECT_EQ(
        bucket.get_container("Container"),
        nullptr);
}

TEST_F(
    container_bucket_unit_tests,
    get_all_containers)
{
    storage::container_bucket bucket{mock_storage_engine_};
    std::vector<std::shared_ptr<storage::container>> containers = bucket.get_all_containers();
    EXPECT_EQ(
        containers.size(),
        0u);

    storage::storage_engine_reference_handle* reference_handle{};
    const schemas::container_persistent_interface container_metadata1 =
        storage::container::create_container_persistent_metadata("Container1");
    EXPECT_EQ(
        bucket.insert_container(reference_handle, container_metadata1),
        status::success);
    containers = bucket.get_all_containers();
    EXPECT_EQ(
        containers.size(),
        1u);
    EXPECT_NE(
        containers.front(),
        nullptr);
    EXPECT_EQ(
        containers.front()->get_name(),
        "Container1");

    const schemas::container_persistent_interface container_metadata2 =
        storage::container::create_container_persistent_metadata("Container2");
    EXPECT_EQ(
        bucket.insert_container(reference_handle, container_metadata2),
        status::success);
    containers = bucket.get_all_containers();
    EXPECT_EQ(
        containers.size(),
        2u);

    bool foundContainer1 = false;
    bool foundContainer2 = false;
    for (const auto& container : containers)
    {
        EXPECT_NE(
            container,
            nullptr);

        if (container->get_name() == "Container1")
        {
            EXPECT_EQ(
                foundContainer1,
                false);

            foundContainer1 = true;
        }
        else if (container->get_name() == "Container2")
        {
            EXPECT_EQ(
                foundContainer2,
                false);

            foundContainer2 = true;
        }
        else
        {
            FAIL();
        }
    }

    EXPECT_EQ(
        bucket.remove_container("Container1"),
        status::success);
    containers = bucket.get_all_containers();
    EXPECT_EQ(
        containers.size(),
        1u);
    EXPECT_NE(
        containers.front(),
        nullptr);
    EXPECT_EQ(
        containers.front()->get_name(),
        "Container2");
}

TEST_F(
    container_bucket_unit_tests,
    remove_container_present)
{
    storage::container_bucket bucket{mock_storage_engine_};
    storage::storage_engine_reference_handle* reference_handle{};
    const schemas::container_persistent_interface container_metadata =
        storage::container::create_container_persistent_metadata("Container");
    EXPECT_EQ(
        bucket.insert_container(reference_handle, container_metadata),
        status::success);
    EXPECT_NE(
        bucket.get_container("Container"),
        nullptr);
    EXPECT_EQ(
        bucket.remove_container("Container"),
        status::success);
    EXPECT_EQ(
        bucket.get_container("Container"),
        nullptr);
}

TEST_F(
    container_bucket_unit_tests,
    remove_container_not_present)
{
    storage::container_bucket bucket{mock_storage_engine_};
    storage::storage_engine_reference_handle* reference_handle{};
    EXPECT_EQ(
        bucket.remove_container("Container"),
        status::container_not_exists);
}

TEST_F(
    container_bucket_unit_tests,
    get_num_containers)
{
    storage::container_bucket bucket{mock_storage_engine_};
    EXPECT_EQ(
        bucket.get_number_containers(),
        0u);

    storage::storage_engine_reference_handle* reference_handle{};
    const schemas::container_persistent_interface container_metadata1 =
        storage::container::create_container_persistent_metadata("Container1");
    bucket.insert_container(reference_handle, container_metadata1);
    EXPECT_EQ(
        bucket.get_number_containers(),
        1u);

    const schemas::container_persistent_interface container_metadata2 =
        storage::container::create_container_persistent_metadata("Container2");
    bucket.insert_container(reference_handle, container_metadata2);
    EXPECT_EQ(
        bucket.get_number_containers(),
        2u);

    EXPECT_EQ(
        bucket.remove_container("Container1"),
        status::success);
    EXPECT_EQ(
        bucket.get_number_containers(),
        1u);
    EXPECT_EQ(
        bucket.remove_container("Container2"),
        status::success);
    EXPECT_EQ(
        bucket.get_number_containers(),
        0u);
}

} // namespace tests.
} // namespace lazarus.