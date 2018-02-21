cmake_minimum_required(VERSION 3.8)
project(transfer_compression)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -O2)
set(SOURCE_FILES lz4.c lz4.h)

add_executable(transfer_compression main.c ${SOURCE_FILES})
add_executable(transfer_compression.client main-client.c ${SOURCE_FILES})