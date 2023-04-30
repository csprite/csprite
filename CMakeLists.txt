cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(csprite VERSION 0.1)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(csprite ${SRC_FILES})
target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

if(TARGET SDL2::SDL2main) # Required on Windows
    target_link_libraries(csprite PUBLIC SDL2::SDL2main)
endif()

target_link_libraries(csprite PUBLIC SDL2::SDL2)
