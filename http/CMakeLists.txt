
#
#设置最低版本号
cmake_minimum_required(VERSION 2.8)
#设置工程名
project(my_http)

message(${PROJECT_SOURCE_DIR})
#设置cpp文件路径
aux_source_directory(${PROJECT_SOURCE_DIR} source_cpp)
#设置头文件路径
set(inc_path ${PROJECT_SOURCE_DIR}) 

message("cpp file:${source_cpp}")

#include()

include_directories(${PROJECT_SOURCE_DIR}../mylib/)

add_executable(my_http ${source_cpp})

target_link_libraries(my_http)

set_target_properties(my_http PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")
