cmake_minimum_required(VERSION 3.8)
project(native_rest_api)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

set(SOURCE_FILES main.c server.c server.h)
add_executable(native_rest_api ${SOURCE_FILES})
add_executable(native_rest_api.client main-client.c)

target_link_libraries(native_rest_api uv)