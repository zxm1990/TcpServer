cmake_minimum_required(VERSION 2.6)

project(server CXX)



include_directories(${PROJECT_SOURCE_DIR})

#boost库头文件的寻找路径
include_directories(/usr/local/include)


add_subdirectory(server/base)
add_subdirectory(server/net)