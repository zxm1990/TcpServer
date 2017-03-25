cmake_minimum_required(VERSION 2.6)

project(server C CXX)

set(CXX_FLAGS
 -g
 -DSERVER_STD_STRING
 )


 string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

include_directories(${PROJECT_SOURCE_DIR})

#boost库头文件的寻找路径
include_directories(/usr/local/include)


add_subdirectory(server/base)
#add_subdirectory(server/net)