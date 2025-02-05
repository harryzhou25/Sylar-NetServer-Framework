cmake_minimum_required(VERSION 3.0)
project(sylar)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(RPCLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src")
set(RPCLIB_TEST_DIR "${CMAKE_SOURCE_DIR}/tests")

include_directories(${RPCLIB_INCLUDE_DIR} ${RPCLIB_TEST_DIR})
add_executable(sylar 
            tests/test_log_basic.cpp 
            src/log/logger.cpp
            src/util/util.cpp)

set_target_properties(
    sylar
    PROPERTIES
    CXX_STANDARD 14
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${RPCLIB_EXTRA_FLAGS}"
)