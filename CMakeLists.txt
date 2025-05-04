cmake_minimum_required(VERSION 3.12)
project(sylar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(utils)

set(LIB_SRC 
    src/log/logger.cpp
    src/config/config.cc

    src/util/util.cpp
    src/util/json_util.cc
    src/util/env.cc
    src/util/hook.cpp
    src/util/daemon.cpp
    src/util/macro.h
    src/util/Singleton.h

    src/thread/thread.cpp
    src/fiber/fiber.cpp
    src/fiber/scheduler.cpp

    src/eventpoller/eventpoller.cpp
    src/timer/timer.cpp

    src/socket/address.cpp
    src/socket/fdManager.cpp
    src/socket/socket.cpp
    src/socket/bytearray.cpp
    src/socket/endian.h
    src/socket/stream.cpp
    src/socket/socketStream.cpp
    src/socket/zlib_stream.cc

    src/rpc/header.pb.cc
    src/rpc/echo_msg.pb.cc

    src/http/http.cc
    src/http/http_connection.cc
    src/http/http_parser.cc
    src/http/http_server.cc
    src/http/http_session.cc
    src/http/servlet.cc
    src/http/session_data.cc
    src/http/servlets/config_servlet.cc

    src/server/TCPserver.cpp
    src/server/RPCserver.cpp

    src/skipList/skipList.cpp
    src/zkClient/zkClient.cpp

    src/skipList/skipList.hpp
    src/skipList/messageQueue.hpp
)

ragelmaker(src/http/http11_parser.rl   LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/http)
ragelmaker(src/http/httpclient_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/http)
ragelmaker(src/util/uri.rl               LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/util)

find_package(ZLIB      REQUIRED)
find_package(yaml-cpp  REQUIRED)
find_package(Protobuf  REQUIRED)

find_library(ZOOKEEPER_LIB
    NAMES zookeeper_mt zookeeper
    PATHS /usr/lib /usr/local/lib
    REQUIRED
)

find_package(PkgConfig  REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

include_directories(
    ${JSONCPP_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests
)

add_executable(test_ollama tests/test_ollama_httpserver.cpp ${LIB_SRC})

target_link_libraries(test_ollama PRIVATE
    ZLIB::ZLIB
    yaml-cpp
    ${PROTOBUF_LIBRARIES}
    ${ZOOKEEPER_LIB}
    ${JSONCPP_LIBRARIES}
)

target_include_directories(test_ollama PRIVATE
    ${Protobuf_INCLUDE_DIRS}
)