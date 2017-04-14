cmake_minimum_required(VERSION 3.6)
project(shakadb)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address  -fno-omit-frame-pointer -Wall")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SRC_DOMAIN src/data-point.h src/fatal-exception.cc src/fatal-exception.h src/storage/standard-data-points-reader.h src/data-point.cc src/configuration.cc src/configuration.h)
set(SRC_CLIENT src/client/session.cc src/client/session.h src/client/read-points-iterator.cc src/client/read-points-iterator.h src/client/client.h src/client/client.cc)
set(SRC_PROTOCOL src/protocol/data-packet.h src/protocol/data-packet.cc src/protocol/write-request.cc src/protocol/write-request.h src/protocol/read-request.cc src/protocol/read-request.h src/protocol/read-response.cc src/protocol/read-response.h src/protocol/write-response.cc src/protocol/write-response.h)
set(SRC_SERVER src/server/client-handler.cc src/server/client-handler.h src/server/server.h src/server/web-server.cc src/server/web-server.h)
set(SRC_STORAGE src/storage/database.h src/storage/data-points-reader.h src/storage/data-chunk.cc src/storage/data-chunk.h src/storage/standard-data-points-reader.cc src/storage/standard-database.cc src/storage/standard-database.h src/storage/data-series.cc src/storage/data-series.h)
set(SRC_UTILS src/log.h src/utils/common.h src/utils/stopwatch.cc src/utils/stopwatch.h src/utils/rw-lock.cc src/utils/rw-lock.h src/utils/rw-lock-scope.cc src/utils/rw-lock-scope.h src/file-log.cc src/file-log.h src/utils/thread.cc src/utils/thread.h src/utils/ring-buffer.cc src/utils/ring-buffer.h src/utils/allocator.h src/utils/allocator.cc src/utils/monitor.cc src/utils/monitor.h src/utils/monitor-scope.cc src/utils/monitor-scope.h src/utils/stream.h src/utils/buffer.h src/utils/memory-buffer.cc src/utils/memory-buffer.h src/utils/shallow-buffer.cc src/utils/shallow-buffer.h src/utils/socket-stream.cc src/utils/socket-stream.h src/utils/disk.h src/utils/disk.c)
set(SRC_TEST test/tests/database-basic-tests.h test/framework/test-context.cc test/framework/test-context.h test/framework/test-runner.cc test/framework/test-runner.h test/framework/assert.cc test/framework/assert.h test/framework/validation-exception.cc test/framework/validation-exception.h test/tests/base-database-tests.h test/tests/database-performance-tests.h test/tests/database-concurrency-tests.h test/tests/rw-lock-tests.h test/tests/null-log.h test/tests/ring-buffer-tests.h test/tests/monitor-tests.h test/tests/configuration-tests.h test/tests/base-database-tests.cc test/tests/configuration-tests.cc test/tests/database-basic-tests.cc test/tests/database-concurrency-tests.cc test/tests/database-performance-tests.cc test/tests/monitor-tests.cc test/tests/ring-buffer-tests.cc test/tests/rw-lock-tests.cc)

set(SRC_SHAKADB ${SRC_DOMAIN} ${SRC_SERVER} ${SRC_PROTOCOL} ${SRC_UTILS} ${SRC_STORAGE})
set(SRC_SHAKADB_CLIENT ${SRC_CLIENT} ${SRC_UTILS} ${SRC_DOMAIN} ${SRC_PROTOCOL})

add_library(shakadbc SHARED ${SRC_SHAKADB_CLIENT})
add_executable(shakadb.client src/client/main.c)
add_executable(shakadb.server ${SRC_SHAKADB} src/main.cc src/bootstrapper.cc src/bootstrapper.h)
add_executable(shakadb.test ${SRC_SHAKADB} ${SRC_SHAKADB_CLIENT} ${SRC_TEST} src/bootstrapper.cc src/bootstrapper.h test/main.cc)

target_link_libraries(shakadbc pthread)
target_link_libraries(shakadb.client pthread shakadbc)
target_link_libraries(shakadb.server pthread)
target_link_libraries(shakadb.test pthread)

set_target_properties(shakadbc PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(shakadb.client PROPERTIES LINKER_LANGUAGE C)
set_target_properties(shakadb.server PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(shakadb.test PROPERTIES LINKER_LANGUAGE CXX)

install(TARGETS shakadbc DESTINATION lib)
install(TARGETS shakadb.server DESTINATION bin)
install(FILES src/client/client.h DESTINATION include/shakadb)