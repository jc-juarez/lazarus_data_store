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

find_package(Drogon REQUIRED)
find_package(Protobuf REQUIRED)

# Define and process .proto files.
set(PROTO_FILES
    src/schemas/object_container_interface.proto
)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# Define source files.
set(SOURCE_FILES
    src/main/main.cc
    src/main/lazarus_data_store.cc
    src/network/server/server.cc
    src/network/endpoints/object_container_endpoint.cc
)

add_executable(lazarus ${SOURCE_FILES} ${PROTO_SRCS})
target_include_directories(lazarus PRIVATE build/)
target_link_libraries(lazarus PRIVATE Drogon::Drogon ${Protobuf_LIBRARIES})