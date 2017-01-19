cmake_minimum_required(VERSION 3.6)
project(apollo-storage)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES src/main.cc src/utils/log.cc src/utils/log.h src/data-point.h src/engine/storage/storage-page.h src/engine/storage/storage.h src/engine/storage/memory-storage-page.cc src/engine/storage/memory-storage-page.h src/engine/storage/memory-storage.cc src/engine/storage/memory-storage.h src/engine/storage/cached-storage-page.cc src/engine/storage/cached-storage-page.h src/engine/storage/cached-storage.cc src/engine/storage/cached-storage.h src/engine/data-chunk.cc src/engine/data-chunk.h src/engine/data-point-reader.cc src/engine/data-point-reader.h src/engine/database.cc src/engine/database.h src/utils/common.h src/utils/file.cc src/utils/file.h)
add_executable(src ${SOURCE_FILES})