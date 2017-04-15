cmake_minimum_required(VERSION 3.6)
project(shakadb)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address  -fno-omit-frame-pointer -Wall -Wpointer-arith")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SRC_DOMAIN src/protocol.h src/protocol.c src/common.h src/common.c)
set(SRC_CLIENT src/client/session.c src/client/session.h src/client/data-points-iterator.c src/client/data-points-iterator.h src/client/client.h src/client/client.c)
set(SRC_SERVER src/server/server.c src/server/server.h)
set(SRC_STORAGE src/storage/data-chunk.c src/storage/data-chunk.h src/storage/data-points-reader.c src/storage/data-points-reader.h src/storage/database.c src/storage/database.h src/storage/data-series.c src/storage/data-series.h)
set(SRC_UTILS src/utils/diagnostics.c src/utils/diagnostics.h src/utils/disk.h src/utils/disk.c src/utils/network.h src/utils/network.c src/utils/memory.h src/utils/memory.c src/utils/threading.h src/utils/threading.c)
set(SRC_TEST test/database-tests.h test/database-tests.c test/framework.c test/framework.h)

set(SRC_SHAKADB ${SRC_DOMAIN} ${SRC_SERVER} ${SRC_UTILS} ${SRC_STORAGE})
set(SRC_SHAKADB_CLIENT ${SRC_CLIENT} ${SRC_UTILS} ${SRC_DOMAIN} ${SRC_PROTOCOL})

add_library(shakadbclientc SHARED ${SRC_SHAKADB_CLIENT})
add_executable(shakadb.server ${SRC_SHAKADB} src/main.c)
add_executable(shakadb.test ${SRC_SHAKADB} ${SRC_SHAKADB_CLIENT} ${SRC_TEST} test/main.c)

target_link_libraries(shakadbclientc pthread)
target_link_libraries(shakadb.server pthread)
target_link_libraries(shakadb.test pthread)

set_target_properties(shakadbclientc PROPERTIES LINKER_LANGUAGE C)
set_target_properties(shakadb.server PROPERTIES LINKER_LANGUAGE C)
set_target_properties(shakadb.test PROPERTIES LINKER_LANGUAGE C)

install(TARGETS shakadbclientc DESTINATION lib)
install(TARGETS shakadb.server DESTINATION bin)
install(FILES src/client/client.h DESTINATION include/shakadb)