cmake_minimum_required(VERSION 3.6)
project(apollo-storage)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES main.cc utils/log.cc utils/log.h data-point.h engine/storage/storage-page.h engine/storage/storage.h engine/storage/memory-storage-page.cc engine/storage/memory-storage-page.h engine/storage/memory-storage.cc engine/storage/memory-storage.h engine/storage/cached-storage-page.cc engine/storage/cached-storage-page.h engine/storage/cached-storage.cc engine/storage/cached-storage.h engine/data-chunk.cc engine/data-chunk.h engine/data-point-reader.cc engine/data-point-reader.h engine/database.cc engine/database.h utils/common.h)
add_executable(src ${SOURCE_FILES})