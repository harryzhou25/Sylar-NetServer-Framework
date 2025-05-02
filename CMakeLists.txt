cmake_minimum_required(VERSION 3.12)
project(sylar LANGUAGES CXX)

# ----------------------
# 基本配置
# ----------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 把自定义模块目录加进来
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(utils)          # 你原来放在 cmake/utils.cmake

# ----------------------
# 源文件列表
# ----------------------
set(LIB_SRC 
    # 日志、配置
    src/log/logger.cpp
    src/config/config.cc

    # 工具
    src/util/util.cpp
    src/util/json_util.cc
    src/util/env.cc
    src/util/hook.cpp
    src/util/daemon.cpp
    src/util/macro.h
    src/util/Singleton.h

    # 线程 & 协程
    src/thread/thread.cpp
    src/fiber/fiber.cpp
    src/fiber/scheduler.cpp

    # 事件 & 定时器
    src/eventpoller/eventpoller.cpp
    src/timer/timer.cpp

    # Socket
    src/socket/address.cpp
    src/socket/fdManager.cpp
    src/socket/socket.cpp
    src/socket/bytearray.cpp
    src/socket/endian.h
    src/socket/stream.cpp
    src/socket/socketStream.cpp
    src/socket/zlib_stream.cc

    # RPC
    src/rpc/header.pb.cc
    src/rpc/echo_msg.pb.cc

    # HTTP
    src/http/http.cc
    src/http/http_connection.cc
    src/http/http_parser.cc
    src/http/http_server.cc
    src/http/http_session.cc
    src/http/servlet.cc
    src/http/session_data.cc
    src/http/servlets/config_servlet.cc

    # Server
    src/server/TCPserver.cpp
    src/server/RPCserver.cpp

    # 其他
    src/skipList/skipList.cpp
    src/zkClient/zkClient.cpp
)

# 如果你用到 ragel 生成 parser，保持原有调用
ragelmaker(src/http/http11_parser.rl   LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/http)
ragelmaker(src/http/httpclient_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/http)
ragelmaker(src/util/uri.rl               LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/util)

# ----------------------
# 查第三方库
# ----------------------
find_package(ZLIB      REQUIRED)
find_package(yaml-cpp  REQUIRED)
find_package(Protobuf  REQUIRED)

find_library(ZOOKEEPER_LIB
    NAMES zookeeper_mt zookeeper
    PATHS /usr/lib /usr/local/lib
    REQUIRED
)

# jsoncpp 没有 CMake config，就用 pkg-config
find_package(PkgConfig  REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# ----------------------
# include dirs
# ----------------------
include_directories(
    ${JSONCPP_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS}
)

# 如果你的 RPC lib 头在 src 目录下
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests
)

# ----------------------
# 定义可执行文件 & 链接
# ----------------------
add_executable(test_ollama tests/test_ollama.cpp ${LIB_SRC})

target_link_libraries(test_ollama PRIVATE
    ZLIB::ZLIB
    yaml-cpp
    ${PROTOBUF_LIBRARIES}
    ${ZOOKEEPER_LIB}
    ${JSONCPP_LIBRARIES}
)

# protobuf 的 include dirs 也可以这样自动加
target_include_directories(test_ollama PRIVATE
    ${Protobuf_INCLUDE_DIRS}
)

# （可选）若有其他可执行文件，类似地 add_executable / target_link_libraries 即可
# add_executable(test_httpserver tests/test_httpserver.cpp ${LIB_SRC})
# target_link_libraries(test_httpserver PRIVATE
#     ZLIB::ZLIB yaml-cpp ${PROTOBUF_LIBRARIES} ${ZOOKEEPER_LIB} ${JSONCPP_LIBRARIES}
# )
