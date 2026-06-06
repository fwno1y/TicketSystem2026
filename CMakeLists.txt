cmake_minimum_required(VERSION 3.10)
project(Code)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 将 include 目录设为“系统目录”之后的优先搜索路径
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES "src/*.cpp")
add_executable(code ${SOURCES})