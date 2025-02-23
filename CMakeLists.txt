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

# Protobuf schemas handling.
set(GENERATED_DIR src/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})
set(PROTO_FILES
    src/schemas/object_container.proto
)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})
include_directories(${GENERATED_DIR} ${Protobuf_INCLUDE_DIRS})

# Source files inclusion.
set(SOURCE_FILES
    src/main/main.cc
    src/main/lazarus_data_store.cc
    src/network/server/server.cc
    src/network/endpoints/object_container_endpoint.cc
)

add_executable(lazarus ${PROTO_SRCS} ${PROTO_HDRS} ${SOURCE_FILES})
target_link_libraries(lazarus PRIVATE Drogon::Drogon ${Protobuf_LIBRARIES})