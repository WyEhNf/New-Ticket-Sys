# CMakeLists.txt 文件
cmake_minimum_required(VERSION 3.10)

project(TicketSystem)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/container)
include_directories(${PROJECT_SOURCE_DIR}/include/System)
include_directories(${PROJECT_SOURCE_DIR}/include/memoryriver)

# 为 MyProject 目标添加包含目录
add_executable(code
 src/system/System.cpp
 src/system/TrainSystem.cpp
src/system/TicketSystem.cpp
src/system/UserSystem.cpp
src/parser.cpp
src/main.cpp)

