cmake_minimum_required(VERSION 3.0)
project(sylar)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(RPCLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src")
set(RPCLIB_TEST_DIR "${CMAKE_SOURCE_DIR}/tests")

set (SRCS 
    tests/test_thread_basic.cpp
    src/log/logger.cpp
    src/util/util.cpp
    src/util/Singleton.h
    src/config/config.cc
    src/util/env.cc
    src/thread/thread.cpp
    src/thread/Mutex.h
)

find_package(yaml-cpp REQUIRED)

include_directories(${RPCLIB_INCLUDE_DIR} ${RPCLIB_TEST_DIR})

add_executable(sylar ${SRCS})

target_link_libraries(sylar PRIVATE ${YAML_CPP_LIBRARIES})

set_target_properties(
    sylar
    PROPERTIES
    CXX_STANDARD 17
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${RPCLIB_EXTRA_FLAGS}"
)

target_compile_options(sylar PRIVATE -g)