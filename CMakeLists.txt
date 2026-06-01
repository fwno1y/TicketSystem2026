cmake_minimum_required(VERSION 3.10)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/storage)
include_directories(${PROJECT_SOURCE_DIR}/src/data)
include_directories(${PROJECT_SOURCE_DIR}/src/manager)
include_directories(${PROJECT_SOURCE_DIR}/src/utils)

# 收集所有源文件
set(UTILS_SOURCES
        src/utils/String.cpp
        src/utils/DateTime.cpp
)

set(DATA_SOURCES
        src/data/Train.cpp
        src/data/Order.cpp
)

set(MANAGER_SOURCES
        src/manager/UserManager.cpp
        src/manager/TrainManager.cpp
        src/manager/OrderManager.cpp
)

set(SYSTEM_SOURCES
        src/CommandParser.cpp
        src/System.cpp
)

# 所有源文件
set(ALL_SOURCES
        ${UTILS_SOURCES}
        ${DATA_SOURCES}
        ${MANAGER_SOURCES}
        ${SYSTEM_SOURCES}
        main.cpp
)

# 生成可执行文件
add_executable(code ${ALL_SOURCES})

# 清理命令
add_custom_target(clean-data
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/data
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.dat
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/BPT*
        COMMENT "Cleaning data files"
)