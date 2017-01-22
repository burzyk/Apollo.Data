cmake_minimum_required(VERSION 3.6)
project(apollo-storage)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SRC_SOURCE_FILES src/utils/log.cc src/utils/log.h src/data-point.h src/engine/storage/storage-page.h src/engine/storage/storage.h src/engine/storage/benchmark/memory-storage-page.cc src/engine/storage/benchmark/memory-storage-page.h src/engine/storage/benchmark/memory-storage.cc src/engine/storage/benchmark/memory-storage.h src/engine/storage/cached-storage-page.cc src/engine/storage/cached-storage-page.h src/engine/storage/cached-storage.cc src/engine/storage/cached-storage.h src/engine/data-chunk.cc src/engine/data-chunk.h src/engine/data-point-reader.cc src/engine/data-point-reader.h src/engine/database.cc src/engine/database.h src/utils/common.h src/utils/file.cc src/utils/file.h src/utils/directory.cc src/utils/directory.h src/utils/stopwatch.cc src/utils/stopwatch.h src/engine/storage/page-allocator.cc src/engine/storage/page-allocator.h src/utils/rw-lock.cc src/utils/rw-lock.h)
set(TEST_SOURCE_FILES test/main.cc test/database-tests.h test/framework/test-context.cc test/framework/test-context.h test/framework/test-runner.cc test/framework/test-runner.h test/framework/assert.cc test/framework/assert.h test/framework/validation-exception.cc test/framework/validation-exception.h)

add_executable(apollo.data ${SRC_SOURCE_FILES} src/main.cc)
add_executable(apollo.test ${TEST_SOURCE_FILES} ${SRC_SOURCE_FILES})

target_link_libraries(apollo.data pthread)
target_link_libraries(apollo.test pthread)