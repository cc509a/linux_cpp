# date: 2017-04-13
#设置最低版本号

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
#设置工程名
PROJECT(my_http)

MESSAGE(${PROJECT_SOURCE_DIR})
#设置cpp文件路径
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR} source_cpp)

#设置头文件路径
SET(inc_path ${PROJECT_SOURCE_DIR}) 
#MESSAGE("cpp file:${source_cpp}")
SET(mylib_path ${PROJECT_SOURCE_DIR}/../mylib/)
#MESSAGE(${mylib_path})

#包含头文件
INCLUDE_DIRECTORIES(${mylib_path})
INCLUDE_DIRECTORIES(${inc_path})

#可执行文件
ADD_EXECUTABLE(my_http ${source_cpp})
#链接库
TARGET_LINK_LIBRARIES(my_http pthread)
#生成目标
SET_TARGET_PROPERTIES(my_http PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")
