cmake_minimum_required(VERSION 2.8.9)
project(SQPKV C CXX)
set(CMAKE_BUILD_TYPE Debug)

#Bring the headers into the project
include_directories(include)
include_directories(src)
include_directories($ENV{HOME}/usr/include)

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -rdynamic -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -Wall -Werror -rdynamic -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(SQPKV_DEPENDENCY_INCLUDE_DIR $ENV{HOME}/usr/include)
set(SQPKV_DEPENDENCY_LIB_DIR $ENV{HOME}/usr/lib)

find_package( Threads REQUIRED )
find_package( ZLIB REQUIRED)
find_package( BZip2 REQUIRED)
find_package( RocksDB REQUIRED )
find_package( Snappy REQUIRED )
find_package( LZ4 REQUIRED )
find_package( Zstd REQUIRED )
find_package( gflags REQUIRED )
find_package( rdma REQUIRED )

set(RDMA_SRCS
    src/rdma/rdma_connection.cc
    src/rdma/rdma_client.cc
    src/rdma/rdma_server.cc)

set(PROTOCOL_SRCS
    src/protocol/net_utils.cc
    src/protocol/packet.cc
    src/protocol/protocol.cc
    src/protocol/status.cc)

set(SERVER_SRCS
    src/server/kv_request_handler.cc
    src/server/kv_worker.cc
    src/server/kv_worker_factory.cc
    src/server/main.cc
    src/server/server.cc
    src/server/server_launcher.cc
    src/server/sharding_policy.cc
    src/server/sharding_proxy_request_handler.cc
    src/server/sharding_proxy_worker.cc
    src/server/sharding_proxy_worker_factory.cc
    src/server/table_prefix_transform.cc
    src/server/worker_thread.cc)

set(CLIENT_SRCS
    src/client/main.cc
    src/client/connection.cc)
 
add_executable(server ${RDMA_SRCS} ${PROTOCOL_SRCS} ${SERVER_SRCS})
target_link_libraries(server
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${ROCKSDB_LIBRARY}
    ${SNAPPY_LIBRARY}
    ${LZ4_LIBRARY}
    ${ZSTD_LIBRARY}
    ${GFLAGS_LIBRARY}
    ${RDMA_LIBRARY}
    ${IBVERBS_LIBRARY} )

add_executable(client ${PROTOCOL_SRCS} ${CLIENT_SRCS})
target_link_libraries(client
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${ROCKSDB_LIBRARY}
    ${SNAPPY_LIBRARY}
    ${LZ4_LIBRARY}
    ${ZSTD_LIBRARY}
    ${GFLAGS_LIBRARY}
    ${RDMA_LIBRARY}
    ${IBVERBS_LIBRARY} )

add_executable(test_server
    ${RDMA_SRCS}
    test/server.cc
    src/protocol/status.cc)
target_link_libraries(test_server
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${ROCKSDB_LIBRARY}
    ${SNAPPY_LIBRARY}
    ${LZ4_LIBRARY}
    ${ZSTD_LIBRARY}
    ${GFLAGS_LIBRARY}
    ${RDMA_LIBRARY}
    ${IBVERBS_LIBRARY} )
 
add_executable(test_client
    ${RDMA_SRCS}
    test/client.cc
    src/protocol/status.cc)
target_link_libraries(test_client
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${ROCKSDB_LIBRARY}
    ${SNAPPY_LIBRARY}
    ${LZ4_LIBRARY}
    ${ZSTD_LIBRARY}
    ${GFLAGS_LIBRARY}
    ${RDMA_LIBRARY}
    ${IBVERBS_LIBRARY} )
