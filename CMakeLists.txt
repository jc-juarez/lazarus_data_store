# *************************************
# Lazarus Data Store
# Build
# 'CMakeLists.txt'
# Author: jcjuarez
# Description:
#      Builds Lazarus Data Store. 
# *************************************

cmake_minimum_required(VERSION 3.28)
project(lazarus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

find_package(TBB REQUIRED)
find_package(Drogon REQUIRED)
find_package(spdlog REQUIRED)
find_package(RocksDB REQUIRED)
find_package(Protobuf REQUIRED)

# Define and process .proto files.
set(PROTO_FILES
        src/schemas/persistent-interfaces/container_persistent_interface.proto
)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# Define source files.
set(SOURCE_FILES
    src/main/main.cc
    src/main/lazarus_data_store.cc
    src/network/server/server.cc
    src/network/endpoints/container_endpoint.cc
    src/network/endpoints/object_endpoint.cc
    src/common/uuid_utilities.cc
    src/storage/container_management_service.cc
    src/storage/storage_engine.cc
    src/storage/garbage_collector.cc
    src/storage/container.cc
    src/storage/container_index.cc
    src/storage/container_operation_serializer.cc
    src/schemas/request-interfaces/container_request.cc
    src/schemas/request-interfaces/object_request.cc
    src/storage/object_management_service.cc
    src/storage/read_io_dispatcher.cc
    src/storage/write_io_dispatcher.cc
    src/common/alertable_sleeper.cc
    src/common/request_validations.cc
    src/common/task_serializer.cc
    src/storage/orphaned_container_scavenger.cc
    src/storage/concurrent_io_dispatcher.cc
    src/status/status_code.hh
    src/common/response_utilities.cc
    src/storage/container_bucket.cc
    src/storage/cache_shard.cc
    src/storage/object.cc
    src/storage/frontline_cache.cc
        src/network/server/request-handlers/container/create_container_request_handler.cc
        src/network/server/request-handlers/container/create_container_request_handler.hh
        src/network/server/request-handlers/container/remove_container_request_handler.cc
        src/network/server/request-handlers/container/remove_container_request_handler.hh
        src/network/server/request-handlers/container/container_request_handler.cc
        src/network/server/request-handlers/container/container_request_handler.hh
        src/network/server/request-handlers/object/insert_object_request_handler.cc
        src/network/server/request-handlers/object/insert_object_request_handler.hh
        src/network/server/request-handlers/object/remove_object_request_handler.cc
        src/network/server/request-handlers/object/remove_object_request_handler.hh
        src/network/server/request-handlers/object/get_object_request_handler.cc
        src/network/server/request-handlers/object/get_object_request_handler.hh
        src/network/server/request-handlers/object/object_request_handler.cc
        src/network/server/request-handlers/object/object_request_handler.hh
    src/common/system_configuration.hh
    src/common/system_configuration.cc
    src/common/args_validations.hh
    src/network/endpoints/ping_endpoint.cc
    src/network/endpoints/ping_endpoint.hh
)

add_executable(lazarus ${SOURCE_FILES} ${PROTO_SRCS})
target_include_directories(lazarus PRIVATE build/)
target_include_directories(lazarus PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(
    lazarus PRIVATE
    Drogon::Drogon
    ${Protobuf_LIBRARIES}
    spdlog::spdlog
    rocksdb
    TBB::tbb
)