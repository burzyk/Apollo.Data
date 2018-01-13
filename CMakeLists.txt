cmake_minimum_required(VERSION 3.2.2)
project(shakadb C)

if (NOT DEFINED ENV{SDB_VERSION_MAJOR})
    set(SDB_VERSION_MAJOR 0)
else ()
    set(SDB_VERSION_MAJOR $ENV{SDB_VERSION_MAJOR})
endif ()

if (NOT DEFINED ENV{SDB_VERSION_MINOR})
    set(SDB_VERSION_MINOR 0)
else ()
    set(SDB_VERSION_MINOR $ENV{SDB_VERSION_MINOR})
endif ()

if (NOT DEFINED ENV{SDB_CONFIGURATION})
    set(SDB_CONFIGURATION debug)
else ()
    set(SDB_CONFIGURATION $ENV{SDB_CONFIGURATION})
endif ()

if (DEFINED ENV{SDB_VERSION_PATCH})
    set(SDB_VERSION_PATCH $ENV{SDB_VERSION_PATCH})
    set(SDB_VERSION "${SDB_VERSION_MAJOR}.${SDB_VERSION_MINOR}.${SDB_VERSION_PATCH}")
    set(SDB_VERSION \\"${SDB_VERSION}\\")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSDB_VERSION=${SDB_VERSION}")
endif ()

if (DEFINED ENV{SDB_BUILD})
    set(SDB_BUILD $ENV{SDB_BUILD})
    set(SDB_BUILD \\"${SDB_BUILD}\\")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSDB_BUILD=${SDB_BUILD}")
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
include_directories(/usr/local/Cellar/libuv/1.9.1/include)

link_directories(/usr/local/Cellar/libuv/1.9.1/lib)
link_directories(/usr/local/lib)

set(SRC_DOMAIN src/protocol.h src/protocol.c src/common.h src/common.c)
set(SRC_CLIENT src/client/session.c src/client/session.h src/client/data-points-iterator.c src/client/data-points-iterator.h src/client/client.h src/client/client.c)
set(SRC_SERVER src/server/server.c src/server/server.h)
set(SRC_STORAGE src/storage/data-chunk.c src/storage/data-chunk.h src/storage/data-points-reader.c src/storage/data-points-reader.h src/storage/database.c src/storage/database.h src/storage/data-series.c src/storage/data-series.h src/storage/cache-manager.c src/storage/cache-manager.h)
set(SRC_UTILS src/utils/diagnostics.c src/utils/diagnostics.h src/utils/disk.h src/utils/disk.c src/utils/network.h src/utils/network.c src/utils/memory.h src/utils/memory.c)
set(SRC_TEST test/database-tests.h test/database-tests.c test/framework.c test/framework.h test/server-tests.c test/server-tests.h test/common-tests.c test/common-tests.h)

set(SRC_SHAKADB ${SRC_DOMAIN} ${SRC_SERVER} ${SRC_UTILS} ${SRC_STORAGE})
set(SRC_SHAKADB_CLIENT ${SRC_CLIENT} ${SRC_UTILS} ${SRC_DOMAIN} ${SRC_PROTOCOL})

add_library(shakadbc SHARED ${SRC_SHAKADB_CLIENT})
add_executable(shakadb ${SRC_SHAKADB} src/main.c)
add_executable(shakadb.client src/client/main.c)
add_executable(shakadb.test ${SRC_SHAKADB} ${SRC_SHAKADB_CLIENT} ${SRC_TEST} test/main-unit.c)
add_executable(shakadb.stress ${SRC_SHAKADB} ${SRC_SHAKADB_CLIENT} ${SRC_TEST} test/main-stress.c)

target_link_libraries(shakadbc dill uv)
target_link_libraries(shakadb dill uv)
target_link_libraries(shakadb.client shakadbc uv)
target_link_libraries(shakadb.test dill pthread uv)
target_link_libraries(shakadb.stress dill pthread uv)

install(TARGETS shakadbc DESTINATION lib)
install(TARGETS shakadb DESTINATION bin)
install(TARGETS shakadb.client DESTINATION bin)
install(FILES src/client/client.h DESTINATION include/shakadb)

# build a CPack driven installer package
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${SDB_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SDB_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${SDB_VERSION_PATCH}")
set(CPACK_PACKAGE_VENDOR "burzyk")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Small and fast time series database")

SET(CPACK_GENERATOR "DEB;TGZ;RPM")
set(CPACK_PACKAGE_CONTACT "p.k.burzynski@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "burzyk")

include(CPack)
