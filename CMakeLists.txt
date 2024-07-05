cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(csprite C CXX)
file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.c)

include(${CMAKE_CURRENT_SOURCE_DIR}/common_defs.cmake)

add_executable(csprite ${SRC_FILES})

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/simpleini/)
#target_link_libraries(csprite PUBLIC SimpleIni)

#set(SFD_BUILD_EXAMPLE OFF CACHE INTERNAL "")
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/sfd/)
#target_link_libraries(csprite PUBLIC Sfd)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/log.c/)
target_link_libraries(csprite PUBLIC rxi_log)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/imbase/)
target_link_libraries(csprite PUBLIC imbase)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/)
target_link_libraries(csprite PUBLIC stb_static)

#set(FILESYSTEM_BUILD_TESTS OFF CACHE INTERNAL "")
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/filesystem/)
#target_link_libraries(csprite PUBLIC FileSystem)

target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_compile_options(csprite PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

if(WIN32)
	add_custom_command(TARGET csprite POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:csprite> $<TARGET_RUNTIME_DLLS:csprite>
		COMMAND_EXPAND_LISTS
	)
endif()

