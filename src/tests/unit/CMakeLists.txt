# *************************************
# Lazarus Data Store
# Tests
# 'CMakeLists.txt'
# Author: jcjuarez
# Description:
#      CMake for unit tests.
# *************************************

add_executable(container_bucket_tests
        container_index_unit_tests.cc
        container_bucket_unit_tests.cc
)

target_include_directories(container_bucket_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(container_bucket_test
        PRIVATE
        GTest::gtest_main
        GTest::gmock
        lazarus_core
)

include(GoogleTest)
gtest_discover_tests(container_bucket_tests)