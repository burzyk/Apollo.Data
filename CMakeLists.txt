cmake_minimum_required(VERSION 3.6)
project(apollo-storage)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(/usr/local/Cellar/libuv/1.9.1/include)

link_directories(/usr/local/Cellar/libuv/1.9.1/lib)

set(CLIENT_SOURCE_FILES src/data-point.h src/server/data-packet.h src/client/session-manager.cc src/client/session-manager.h src/client/session.cc src/client/session.h)
set(SRC_SOURCE_FILES src/utils/log.h src/data-point.h src/storage/data-chunk.cc src/storage/data-chunk.h src/storage/data-point-reader.cc src/storage/data-point-reader.h src/storage/database.cc src/storage/database.h src/utils/common.h src/utils/file.cc src/utils/file.h src/utils/directory.cc src/utils/directory.h src/utils/stopwatch.cc src/utils/stopwatch.h src/utils/rw-lock.cc src/utils/rw-lock.h src/utils/rw-lock-scope.cc src/utils/rw-lock-scope.h src/fatal-exception.cc src/fatal-exception.h src/utils/file-log.cc src/utils/file-log.h src/storage/data-series.cc src/storage/data-series.h src/server/data-packet.h src/server/server.h src/server/client-handler.h src/server/uv-server.cc src/server/uv-server.h src/server/handlers/packet-logger.cc src/server/handlers/packet-logger.h src/server/client-responder.h src/utils/thread.cc src/utils/thread.h)
set(TEST_SOURCE_FILES test/main.cc test/storage/unit-tests.h test/framework/test-context.cc test/framework/test-context.h test/framework/test-runner.cc test/framework/test-runner.h test/framework/assert.cc test/framework/assert.h test/framework/validation-exception.cc test/framework/validation-exception.h test/storage/common.h test/storage/performance-tests.h test/storage/concurrency-tests.h test/utils/rw-lock-test.h test/server/server-tests.h test/common.h)

add_executable(apollo.client ${CLIENT_SOURCE_FILES} src/client/main.cc)
add_executable(apollo.data ${SRC_SOURCE_FILES} src/main.cc )
add_executable(apollo.test ${TEST_SOURCE_FILES} ${SRC_SOURCE_FILES})

target_link_libraries(apollo.data pthread)
target_link_libraries(apollo.test pthread)

target_link_libraries(apollo.data uv)
target_link_libraries(apollo.test uv)