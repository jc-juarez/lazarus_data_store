# *************************************
# Lazarus Data Store
# Build
# 'CMakeLists.txt'
# Author: jcjuarez
# Description:
#      Builds Lazarus Data Store. 
# *************************************

cmake_minimum_required(VERSION 3.10)
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
    src/schemas/persistent-interfaces/object_container_persistent_interface.proto
)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# Define source files.
set(SOURCE_FILES
    src/main/main.cc
    src/main/lazarus_data_store.cc
    src/network/server/server.cc
    src/network/endpoints/object_container_endpoint.cc
    src/common/uuid_utilities.cc
    src/storage/object_container_management_service.cc
    src/storage/storage_engine.cc
    src/storage/garbage_collector.cc
    src/storage/object_container.cc
    src/storage/object_container_index.cc
    src/storage/object_container_operation_serializer.cc
    src/schemas/request-interfaces/object_container_request_interface.cc
)

add_executable(lazarus ${SOURCE_FILES} ${PROTO_SRCS})
target_include_directories(lazarus PRIVATE build/)
target_link_libraries(
    lazarus PRIVATE
    Drogon::Drogon
    ${Protobuf_LIBRARIES}
    spdlog::spdlog
    rocksdb
    TBB::tbb
)