cmake_minimum_required(VERSION 3.18)


project(respcpp)


project(resp)

set(CMAKE_CXX_COMPILER "c++")
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)

# 设定编译的FLAGS
set(CMAKE_CXX_FLAGS -g -W -lsockpp)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: " "${CMAKE_CXX_FLAGS}")

include_directories(.)
include_directories(/usr/local/include)

link_directories(/usr/local/lib)


add_executable(main main.cpp resp.hpp bufio.hpp)
add_executable(server server.cpp)

target_link_libraries(server /usr/local/lib/libsockpp.so)