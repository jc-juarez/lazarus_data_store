cmake_minimum_required(VERSION 3.10)
project(lazarus)

set(CMAKE_CXX_STANDARD 20)

find_package(Drogon REQUIRED)

set(SOURCE_FILES
    # Server components.
    src/main/main.cc
    src/main/lazarus_data_store.cc
    src/network/server/server.cc
    src/network/endpoints/object_container_endpoint.cc
)

add_executable(lazarus ${SOURCE_FILES})

target_link_libraries(lazarus PRIVATE Drogon::Drogon)
