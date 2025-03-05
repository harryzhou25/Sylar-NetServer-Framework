cmake_minimum_required(VERSION 3.0)
project(sylar)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(RPCLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src")
set(RPCLIB_TEST_DIR "${CMAKE_SOURCE_DIR}/tests")

set (SRCS 
    tests/test_ConcurVec.cpp
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
)

find_package(yaml-cpp REQUIRED)

find_library(ZOOKEEPER_LIB
    NAMES zookeeper_mt zookeeper
    PATHS /usr/lib /usr/local/lib
)

include_directories(${RPCLIB_INCLUDE_DIR} ${RPCLIB_TEST_DIR})

add_executable(sylar ${SRCS})

target_link_libraries(sylar PRIVATE ${YAML_CPP_LIBRARIES} ${ZOOKEEPER_LIB})

set_target_properties(
    sylar
    PROPERTIES
    CXX_STANDARD 17
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${RPCLIB_EXTRA_FLAGS}"
)

target_compile_options(sylar PRIVATE -g)