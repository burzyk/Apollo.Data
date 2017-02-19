cmake_minimum_required(VERSION 3.6)
project(shakadb)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(/usr/local/Cellar/libuv/1.9.1/include)

link_directories(/usr/local/Cellar/libuv/1.9.1/lib)


set(SRC_DOMAIN src/data-point.h src/fatal-exception.cc src/fatal-exception.h)
set(SRC_CLIENT src/client/session-manager.cc src/client/session-manager.h src/client/session.cc src/client/session.h)
set(SRC_MIDDLEWARE src/middleware/packet-logger.cc src/middleware/packet-logger.h src/middleware/ping-handler.cc src/middleware/ping-handler.h src/middleware/write-handler.cc src/middleware/write-handler.h src/middleware/base-handler.cc src/middleware/base-handler.h)
set(SRC_PROTOCOL src/protocol/data-packet.h src/protocol/data-packet.cc src/protocol/ping-packet.cc src/protocol/ping-packet.h src/protocol/write-request.cc src/protocol/write-request.h src/protocol/packet-loader.cc src/protocol/packet-loader.h src/protocol/read-request.cc src/protocol/read-request.h src/protocol/read-response.cc src/protocol/read-response.h)
set(SRC_SERVER src/server/server.h src/server/uv-server.cc src/server/uv-server.h src/server/uv-server-client.cc src/server/uv-server-client.h src/server/server-client.h src/server/uv-common.h src/server/uv-common.cc)
set(SRC_STORAGE src/storage/data-chunk.cc src/storage/data-chunk.h src/storage/standard-data-point-reader.cc src/storage/standard-data-point-reader.h src/storage/standard-database.cc src/storage/standard-database.h src/storage/data-series.cc src/storage/data-series.h src/storage/database.h src/storage/data-point-reader.h)
set(SRC_UTILS src/log.h src/utils/common.h src/utils/file.cc src/utils/file.h src/utils/directory.cc src/utils/directory.h src/utils/stopwatch.cc src/utils/stopwatch.h src/utils/rw-lock.cc src/utils/rw-lock.h src/utils/rw-lock-scope.cc src/utils/rw-lock-scope.h src/file-log.cc src/file-log.h src/utils/thread.cc src/utils/thread.h src/utils/ring-buffer.cc src/utils/ring-buffer.h src/utils/allocator.h src/utils/allocator.cc src/utils/monitor.cc src/utils/monitor.h src/utils/monitor-scope.cc src/utils/monitor-scope.h src/utils/stream.h)
set(SRC_TEST test/storage/unit-tests.h test/framework/test-context.cc test/framework/test-context.h test/framework/test-runner.cc test/framework/test-runner.h test/framework/assert.cc test/framework/assert.h test/framework/validation-exception.cc test/framework/validation-exception.h test/storage/common.h test/storage/performance-tests.h test/storage/concurrency-tests.h test/utils/rw-lock-tests.h test/server/server-tests.h test/common.h test/utils/ring-buffer-tests.h test/utils/monitor-tests.h)

set(SRC_SHAKADB ${SRC_DOMAIN} ${SRC_MIDDLEWARE} ${SRC_SERVER} ${SRC_PROTOCOL} ${SRC_UTILS} ${SRC_STORAGE})
set(SRC_SHAKADB_CLIENT ${SRC_CLIENT} ${SRC_UTILS} ${SRC_DOMAIN} ${SRC_PROTOCOL})

add_executable(shakadb.client ${SRC_SHAKADB_CLIENT} src/client/main.cc)
add_executable(shakadb ${SRC_SHAKADB} src/main.cc src/bootstrapper.cc src/bootstrapper.h)
add_executable(shakadb.test ${SRC_TEST} ${SRC_SHAKADB} ${SRC_SHAKADB_CLIENT} test/main.cc)

target_link_libraries(shakadb pthread uv)
target_link_libraries(shakadb.test pthread uv)