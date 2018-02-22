cmake_minimum_required(VERSION 3.2.2)
project(shakadb C)

if (NOT DEFINED ENV{SDB_CONFIGURATION})
    set(SDB_CONFIGURATION debug)
else ()
    set(SDB_CONFIGURATION $ENV{SDB_CONFIGURATION})
endif ()

if (DEFINED ENV{SDB_VERSION})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSDB_VERSION='\"$ENV{SDB_VERSION}\"'")
endif ()

if (DEFINED ENV{SDB_BUILD})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSDB_BUILD='\"$ENV{SDB_BUILD}\"'")
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpointer-arith")

if ("${SDB_CONFIGURATION}" STREQUAL debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif ()

if ("${SDB_CONFIGURATION}" STREQUAL release)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
endif ()

message("===== building in " ${SDB_CONFIGURATION} " =====")
message("flags: ${CMAKE_C_FLAGS}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(/usr/local/include)

link_directories(/usr/local/lib)

set(SRC_COMMON src/network/protocol.h src/network/protocol.c src/common.h src/common.c src/diagnostics.c src/diagnostics.h src/data-point.c src/data-point.h)
set(SRC_CTL src/ctl/session.c src/ctl/session.h)
set(SRC_SERVER src/network/server.c src/network/server.h src/network/client-handler.c src/network/client-handler.h)
set(SRC_STORAGE src/storage/disk.h src/storage/disk.c src/storage/points-reader.c src/storage/points-reader.h src/storage/database.c src/storage/database.h src/storage/series.c src/storage/series.h)
set(SRC_TEST test/database-tests.h test/database-tests.c test/framework.c test/framework.h test/server-tests.c test/server-tests.h test/common-tests.c test/common-tests.h)

set(SRC_SHAKADB ${SRC_COMMON} ${SRC_SERVER} ${SRC_STORAGE})
set(SRC_SHAKADB_CTL ${SRC_COMMON} ${SRC_CTL} )

add_executable(shakadb ${SRC_SHAKADB} src/main.c)
add_executable(shakadbctl ${SRC_SHAKADB_CTL} src/ctl/main.c)
add_executable(shakadb.test ${SRC_SHAKADB} ${SRC_SHAKADB_CTL} ${SRC_TEST} test/main-unit.c)
add_executable(shakadb.stress ${SRC_SHAKADB_CTL} test/main-stress.c)

target_link_libraries(shakadb uv)
target_link_libraries(shakadbctl)
target_link_libraries(shakadb.test pthread uv)
target_link_libraries(shakadb.stress)

install(TARGETS shakadb DESTINATION bin)
install(TARGETS shakadbctl DESTINATION bin)
