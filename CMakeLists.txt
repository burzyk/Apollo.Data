cmake_minimum_required(VERSION 3.6)
project(shakadb)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address  -fno-omit-frame-pointer -Wall -Wpointer-arith")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address  -fno-omit-frame-pointer -Wall -Wpointer-arith")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SRC_DOMAIN src/storage/data-points-reader.h src/common.h src/common.c)
set(SRC_CLIENT src/client/session.cc src/client/session.h src/client/data-points-iterator.c src/client/data-points-iterator.h src/client/client.h src/client/client.cc)
set(SRC_PROTOCOL src/protocol.h src/protocol.c)
set(SRC_SERVER src/server/server.c src/server/server.h)
set(SRC_STORAGE src/storage/data-chunk.c src/storage/data-chunk.h src/storage/data-points-reader.c src/storage/database.c src/storage/database.h src/storage/data-series.c src/storage/data-series.h)
set(SRC_UTILS src/utils/disk.h src/utils/disk.c src/utils/network.h src/utils/memory.h src/utils/memory.c src/utils/network.c src/utils/threading.h src/utils/threading.c)
set(SRC_TEST test/database-tests.h test/database-tests.cc src/protocol.h src/utils/diagnostics.c src/utils/diagnostics.h test/framework.c test/framework.h)

set(SRC_SHAKADB ${SRC_DOMAIN} ${SRC_SERVER} ${SRC_PROTOCOL} ${SRC_UTILS} ${SRC_STORAGE})
set(SRC_SHAKADB_CLIENT ${SRC_CLIENT} ${SRC_UTILS} ${SRC_DOMAIN} ${SRC_PROTOCOL})

add_library(shakadbclientc SHARED ${SRC_SHAKADB_CLIENT})
add_library(shakadbserverc SHARED ${SRC_SHAKADB})
add_executable(shakadb.client src/client/main.c)
add_executable(shakadb.test ${SRC_SHAKADB} ${SRC_SHAKADB_CLIENT} ${SRC_TEST} test/main.c test/framework.c test/framework.h)

target_link_libraries(shakadbclientc pthread)
target_link_libraries(shakadbserverc pthread)
target_link_libraries(shakadb.client pthread shakadbclientc)
target_link_libraries(shakadb.test pthread)

set_target_properties(shakadbclientc PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(shakadbserverc PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(shakadb.client PROPERTIES LINKER_LANGUAGE C)
set_target_properties(shakadb.test PROPERTIES LINKER_LANGUAGE CXX)

install(TARGETS shakadbclientc DESTINATION lib)
install(TARGETS shakadbserverc DESTINATION lib)
install(FILES src/client/client.h DESTINATION include/shakadb)