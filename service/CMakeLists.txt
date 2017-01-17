cmake_minimum_required(VERSION 3.6)
project(apollo-storage)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES main.cc utils/log.cc utils/log.h data-point.h engine/storage-page.h engine/storage.h engine/memory-storage-page.cc engine/memory-storage-page.h engine/memory-storage.cc engine/memory-storage.h)
add_executable(src ${SOURCE_FILES})