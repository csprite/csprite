cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(imbase C CXX)

file(GLOB SRC_FILES src/window.cpp src/texture.cpp src/launcher.cpp)
include(${CMAKE_CURRENT_SOURCE_DIR}/common_defs.cmake)
add_library(imbase STATIC ${SRC_FILES})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/)

target_include_directories(imbase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include/)
target_link_libraries(imbase PUBLIC glad_loader)
target_link_libraries(imbase PUBLIC glfw)
target_link_libraries(imbase PUBLIC imgui_static)
target_link_libraries(imgui_static PUBLIC glfw)

target_compile_options(imbase PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

