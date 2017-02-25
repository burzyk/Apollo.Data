cmake_minimum_required(VERSION 3.6)
project(shakadb)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SRC_DOMAIN src/database.h src/data-points-reader.h src/data-point.h src/fatal-exception.cc src/fatal-exception.h src/storage/standard-data-points-reader.h src/data-point.cc src/configuration.cc src/configuration.h)
set(SRC_CLIENT src/client/session.cc src/client/session.h src/client/read-points-iterator.cc src/client/read-points-iterator.h)
set(SRC_MIDDLEWARE src/middleware/packet-logger.cc src/middleware/packet-logger.h src/middleware/ping-handler.cc src/middleware/ping-handler.h src/middleware/write-handler.cc src/middleware/write-handler.h src/middleware/base-handler.cc src/middleware/base-handler.h src/middleware/read-handler.cc src/middleware/read-handler.h)
set(SRC_PROTOCOL src/protocol/data-packet.h src/protocol/data-packet.cc src/protocol/ping-packet.cc src/protocol/ping-packet.h src/protocol/write-request.cc src/protocol/write-request.h src/protocol/read-request.cc src/protocol/read-request.h src/protocol/read-response.cc src/protocol/read-response.h src/protocol/write-response.cc src/protocol/write-response.h)
set(SRC_SERVER src/server/server.h src/server/web-server.cc src/server/web-server.h)
set(SRC_STORAGE src/storage/data-chunk.cc src/storage/data-chunk.h src/storage/standard-data-points-reader.cc src/storage/standard-database.cc src/storage/standard-database.h src/storage/data-series.cc src/storage/data-series.h)
set(SRC_UTILS src/log.h src/utils/common.h src/utils/file.cc src/utils/file.h src/utils/directory.cc src/utils/directory.h src/utils/stopwatch.cc src/utils/stopwatch.h src/utils/rw-lock.cc src/utils/rw-lock.h src/utils/rw-lock-scope.cc src/utils/rw-lock-scope.h src/file-log.cc src/file-log.h src/utils/thread.cc src/utils/thread.h src/utils/ring-buffer.cc src/utils/ring-buffer.h src/utils/allocator.h src/utils/allocator.cc src/utils/monitor.cc src/utils/monitor.h src/utils/monitor-scope.cc src/utils/monitor-scope.h src/utils/stream.h src/utils/common.cc src/utils/buffer.h src/utils/memory-buffer.cc src/utils/memory-buffer.h src/utils/shallow-buffer.cc src/utils/shallow-buffer.h src/utils/socket-stream.cc src/utils/socket-stream.h)
set(SRC_TEST test/tests/database-basic-tests.h test/framework/test-context.cc test/framework/test-context.h test/framework/test-runner.cc test/framework/test-runner.h test/framework/assert.cc test/framework/assert.h test/framework/validation-exception.cc test/framework/validation-exception.h test/tests/common.h test/tests/database-performance-tests.h test/tests/database-concurrency-tests.h test/tests/rw-lock-tests.h test/common.h test/tests/ring-buffer-tests.h test/tests/monitor-tests.h test/tests/configuration-tests.h test/tests/end-to-end.h)

set(SRC_SHAKADB ${SRC_DOMAIN} ${SRC_MIDDLEWARE} ${SRC_SERVER} ${SRC_PROTOCOL} ${SRC_UTILS} ${SRC_STORAGE})
set(SRC_SHAKADB_CLIENT ${SRC_CLIENT} ${SRC_UTILS} ${SRC_DOMAIN} ${SRC_PROTOCOL})

add_executable(shakadb.client ${SRC_SHAKADB_CLIENT} src/client/main.cc)
add_executable(shakadb ${SRC_SHAKADB} src/main.cc src/bootstrapper.cc src/bootstrapper.h)
add_executable(shakadb.test ${SRC_SHAKADB} ${SRC_SHAKADB_CLIENT} ${SRC_TEST} src/bootstrapper.cc src/bootstrapper.h test/main.cc)

target_link_libraries(shakadb pthread)
target_link_libraries(shakadb.test pthread)