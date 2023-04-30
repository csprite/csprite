cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(csprite VERSION 0.1)
add_executable(csprite src/main.cpp)
target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
