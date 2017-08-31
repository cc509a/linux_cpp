# date: 2017-04-13
#设置最低版本号

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#set(CMAKE_CXX_COMPILER "/usr/local/bin/g++")
ADD_COMPILE_OPTIONS(-std=c++11 -g -Wall -O2 )
#设置工程名
PROJECT(ECHO)

#MESSAGE(${PROJECT_SOURCE_DIR})
#设置cpp文件路径
#AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR} server.cc)

#设置头文件路径
#SET(inc_path ${PROJECT_SOURCE_DIR}) 
#MESSAGE("cpp file:${source_cpp}")
SET(mylib_path ${PROJECT_SOURCE_DIR}/../mylib/)
#MESSAGE(${mylib_path})

#包含头文件
INCLUDE_DIRECTORIES(${mylib_path})
#INCLUDE_DIRECTORIES(${inc_path})

#可执行文件
ADD_EXECUTABLE(server_echo server.cc)
#链接库
TARGET_LINK_LIBRARIES(server_echo pthread)
#生成目标
SET_TARGET_PROPERTIES(server_echo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")



#可执行文件
ADD_EXECUTABLE(cilent_echo client.cc)
#链接库
TARGET_LINK_LIBRARIES(cilent_echo pthread)
#生成目标
SET_TARGET_PROPERTIES(cilent_echo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")