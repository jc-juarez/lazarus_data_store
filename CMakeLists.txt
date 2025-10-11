# ****************************************************
# Lazarus Data Store
# Build
# 'CMakeLists.txt'
# Author: jcjuarez
# Description:
#      Root CMake for Lazarus Data Store.
# ****************************************************

cmake_minimum_required(VERSION 3.28)
project(lazarus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

find_package(TBB REQUIRED)
find_package(Drogon REQUIRED)
find_package(spdlog REQUIRED)
find_package(RocksDB REQUIRED)
find_package(Protobuf REQUIRED)
find_path(JSONCPP_INCLUDE_DIR json/json.h
        PATHS /usr/include /usr/include/jsoncpp
)
find_library(JSONCPP_LIBRARY jsoncpp)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

set(PROTO_FILES
        src/core/schemas/persistent-interfaces/container_persistent_interface.proto
)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

set(SOURCE_FILES
        src/core/startup/lazarus_data_store.cc
        src/core/network/server/server.cc
        src/core/network/endpoints/containers.cc
        src/core/network/endpoints/objects.cc
        src/core/common/uuid_utilities.cc
        src/core/storage/container_management_service.cc
        src/core/storage/storage_engine.cc
        src/core/storage/garbage_collector.cc
        src/core/storage/container.cc
        src/core/storage/container_index.cc
        src/core/storage/container_operation_serializer.cc
        src/core/schemas/request-interfaces/container_request.cc
        src/core/schemas/request-interfaces/object_request.cc
        src/core/storage/object_management_service.cc
        src/core/storage/read_io_dispatcher.cc
        src/core/storage/write_io_dispatcher.cc
        src/core/common/alertable_sleeper.cc
        src/core/common/request_validations.cc
        src/core/common/task_serializer.cc
        src/core/storage/orphaned_container_scavenger.cc
        src/core/storage/concurrent_io_dispatcher.cc
        src/core/status/status_code.hh
        src/core/common/response_utilities.cc
        src/core/storage/container_bucket.cc
        src/core/storage/cache_shard.cc
        src/core/storage/object.cc
        src/core/storage/frontline_cache.cc
        src/core/network/server/request-handlers/container/create_container_request_handler.cc
        src/core/network/server/request-handlers/container/create_container_request_handler.hh
        src/core/network/server/request-handlers/container/remove_container_request_handler.cc
        src/core/network/server/request-handlers/container/remove_container_request_handler.hh
        src/core/network/server/request-handlers/container/container_request_handler.cc
        src/core/network/server/request-handlers/container/container_request_handler.hh
        src/core/network/server/request-handlers/object/insert_object_request_handler.cc
        src/core/network/server/request-handlers/object/insert_object_request_handler.hh
        src/core/network/server/request-handlers/object/remove_object_request_handler.cc
        src/core/network/server/request-handlers/object/remove_object_request_handler.hh
        src/core/network/server/request-handlers/object/get_object_request_handler.cc
        src/core/network/server/request-handlers/object/get_object_request_handler.hh
        src/core/network/server/request-handlers/object/object_request_handler.cc
        src/core/network/server/request-handlers/object/object_request_handler.hh
        src/core/common/system_configuration.hh
        src/core/common/system_configuration.cc
        src/core/common/args_validations.hh
        src/core/network/endpoints/ping.cc
        src/core/network/endpoints/ping.hh
        src/core/storage/storage_engine_interface.hh
        src/core/common/interface.hh
)

# Lazarus core library.
add_library(lazarus_core ${SOURCE_FILES} ${PROTO_SRCS})
target_include_directories(lazarus_core PRIVATE
        build/
        ${CMAKE_CURRENT_BINARY_DIR}
        ${JSONCPP_INCLUDE_DIR}
)
target_link_libraries(
        lazarus_core PUBLIC
        Drogon::Drogon
        ${Protobuf_LIBRARIES}
        spdlog::spdlog
        rocksdb
        TBB::tbb
        ${JSONCPP_LIBRARY}
)

# Lazarus executable.
add_executable(lazarus src/main/main.cc)
target_link_libraries(lazarus PRIVATE lazarus_core)

# Lazarus tests.
add_executable(lazarus_tests
        src/tests/mocks/mock_storage_engine.hh
        src/tests/unit/container_index_unit_tests.cc
        src/tests/unit/container_bucket_unit_tests.cc
)
target_include_directories(lazarus_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(lazarus_tests PRIVATE
        lazarus_core
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(lazarus_tests)