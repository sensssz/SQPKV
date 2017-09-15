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

set(ROCKSDB_LIB
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${ROCKSDB_LIBRARY}
    ${SNAPPY_LIBRARY}
    ${LZ4_LIBRARY}
    ${ZSTD_LIBRARY})

set(RDMA_SRCS
    src/rdma/rdma_connection.cc
    src/rdma/rdma_client.cc
    src/rdma/rdma_server.cc)

set(PROTOCOL_SRCS
    src/protocol/packet.cc
    src/protocol/protocol.cc
    src/protocol/status.cc)

set(ROUTER_SRCS
    src/router/exponential_speculator.cc
    src/router/prefetch_cache.cc
    src/router/router_kv_request_handler.cc
    src/router/router_worker_factory.cc
    src/router/router_worker.cc
    src/router/server.cc
    src/router/sharding_policy.cc
    src/router/sqp_request_handler.cc)

set(SERVER_SRCS
    src/server/kv_request_handler.cc
    src/server/main.cc
    src/server/server_launcher.cc
    src/server/table_prefix_transform.cc)

set(UTILS_SRCS
    src/utils/key_splitter.cc
    src/utils/net_utils.cc)

set(CONNECTOR_SRCS
    src/connector/connection.cc
    ${PROTOCOL_SRCS})

set(CLIENT_SRCS
    src/client/main.cc)

set(EXPONENTIAL_SRCS
    src/client/exponential_client.cc
    src/client/exponential_distribution.cc
    src/client/random_generator.cc)

add_library(sqpkv_connector SHARED
    ${CONNECTOR_SRCS}
    ${UTILS_SRCS})
    
add_executable(sqpkv_server
    ${RDMA_SRCS}
    ${PROTOCOL_SRCS}
    ${ROUTER_SRCS}
    ${SERVER_SRCS}
    ${UTILS_SRCS})
target_link_libraries(sqpkv_server
    ${CMAKE_THREAD_LIBS_INIT}
    ${ROCKSDB_LIB}
    ${GFLAGS_LIBRARY}
    ${RDMA_LIBRARY}
    ${IBVERBS_LIBRARY} )

add_executable(sqpkv_client ${CLIENT_SRCS})
target_link_libraries(sqpkv_client
    sqpkv_connector
    ${CMAKE_THREAD_LIBS_INIT}
    ${ROCKSDB_LIB}
    ${GFLAGS_LIBRARY}
    ${RDMA_LIBRARY}
    ${IBVERBS_LIBRARY} )


add_executable(exponential_client ${EXPONENTIAL_SRCS})
target_link_libraries(exponential_client
    sqpkv_connector
    ${CMAKE_THREAD_LIBS_INIT}
    ${ROCKSDB_LIB}
    ${GFLAGS_LIBRARY}
    ${RDMA_LIBRARY}
    ${IBVERBS_LIBRARY} )

add_executable(rdma_test_server
    ${RDMA_SRCS}
    test/rdma_server.cc
    src/protocol/status.cc)
target_link_libraries(rdma_test_server
    ${CMAKE_THREAD_LIBS_INIT}
    ${ROCKSDB_LIB}
    ${GFLAGS_LIBRARY}
    ${RDMA_LIBRARY}
    ${IBVERBS_LIBRARY} )
 
add_executable(rdma_test_client
    ${RDMA_SRCS}
    test/rdma_client.cc
    src/protocol/status.cc)
target_link_libraries(rdma_test_client
    ${CMAKE_THREAD_LIBS_INIT}
    ${ROCKSDB_LIB}
    ${GFLAGS_LIBRARY}
    ${RDMA_LIBRARY}
    ${IBVERBS_LIBRARY} )

add_executable(echo_server
    test/echo_server.cc
    ${UTILS_SRCS} )
target_link_libraries(echo_server
    ${GFLAGS_LIBRARY} )
 
add_executable(echo_client
    test/echo_client.cc
    ${UTILS_SRCS} )
target_link_libraries(echo_client
    ${GFLAGS_LIBRARY} )

file(GLOB CONNECTOR_HEADERS include/sqpkv/*.h)
install(FILES ${CONNECTOR_HEADERS} DESTINATION include/sqpkv/)
install(TARGETS sqpkv_connector DESTINATION lib/)
install(TARGETS sqpkv_server DESTINATION bin/)
install(TARGETS sqpkv_client DESTINATION bin/)
