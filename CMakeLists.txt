cmake_minimum_required(VERSION 3.0)
project(sylar)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(RPCLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src")
set(RPCLIB_TEST_DIR "${CMAKE_SOURCE_DIR}/tests")

set (SRCS 
    src/log/logger.cpp
    src/util/util.cpp
    src/util/Singleton.h
    src/util/hook.cpp
    src/util/env.cc
    src/util/macro.h
    src/util/daemon.cpp
    src/config/config.cc
    src/thread/thread.cpp
    src/thread/Mutex.h
    src/fiber/fiber.cpp
    src/fiber/scheduler.cpp
    src/eventpoller/eventpoller.cpp
    src/timer/timer.cpp
    src/socket/endian.h
    src/socket/fdManager.cpp
    src/socket/address.cpp
    src/socket/socket.cpp
    src/socket/bytearray.cpp
    src/server/TCPserver.cpp
    src/server/RPCserver.cpp
    src/rpc/header.pb.cc
    src/rpc/echo_msg.pb.cc
    src/zkClient/zkClient.cpp

    src/skipList/skipList.hpp
    src/skipList/messageQueue.hpp
)

find_package(yaml-cpp REQUIRED)

find_package(Protobuf REQUIRED)

find_library(ZOOKEEPER_LIB
    NAMES zookeeper_mt zookeeper
    PATHS /usr/lib /usr/local/lib
)

include_directories(${RPCLIB_INCLUDE_DIR} ${RPCLIB_TEST_DIR})# ${PROTOBUF_INCLUDE_DIRS})

add_executable(test_skList tests/test_messageQueue.cpp ${SRCS})

add_executable(client samples/EchoServer/client.cpp ${SRCS})

add_executable(server samples/EchoServer/server.cpp ${SRCS})

target_link_libraries(test_skList PRIVATE ${YAML_CPP_LIBRARIES} ${ZOOKEEPER_LIB} ${PROTOBUF_LIBRARIES})

target_link_libraries(client PRIVATE ${YAML_CPP_LIBRARIES} ${ZOOKEEPER_LIB} ${PROTOBUF_LIBRARIES})

target_link_libraries(server PRIVATE ${YAML_CPP_LIBRARIES} ${ZOOKEEPER_LIB} ${PROTOBUF_LIBRARIES})

set_target_properties(client PROPERTIES
    CXX_STANDARD 17
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${RPCLIB_EXTRA_FLAGS}"
)

target_compile_options(client PRIVATE -g -lprotobuf)
target_compile_options(server PRIVATE -g -lprotobuf)
target_compile_options(test_skList PRIVATE -g -lprotobuf)