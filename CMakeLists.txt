cmake_minimum_required(VERSION 3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(csprite C CXX)
add_definitions(-DSDL_MAIN_HANDLED)

# Debug C/C++ flags
if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_compile_options(-fsanitize=address)
	add_link_options(-fsanitize=address)
	add_definitions(-D_DEBUG)
else()
	add_definitions(-D_NDEBUG)
endif()

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.c)
add_executable(csprite ${SRC_FILES})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
target_link_libraries(csprite PUBLIC third_party)

target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(ZLIB)
target_link_libraries(csprite PUBLIC ZLIB::ZLIB)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

if(TARGET SDL2::SDL2main) # Required on Windows
	target_link_libraries(csprite PUBLIC SDL2::SDL2main)
endif()

target_link_libraries(third_party PUBLIC SDL2::SDL2)
target_link_libraries(csprite PUBLIC SDL2::SDL2)

if(CMAKE_CXX_BYTE_ORDER STREQUAL "BIG_ENDIAN")
	add_definitions(-DTARGET_IS_BIGENDIAN=1)
else()
	add_definitions(-DTARGET_IS_BIGENDIAN=0)
endif()

if(WIN32)
	add_custom_command(TARGET csprite POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:csprite> $<TARGET_RUNTIME_DLLS:csprite>
		COMMAND_EXPAND_LISTS
	)
	add_definitions(-DTARGET_WINDOWS)
elseif(LINUX)
	add_definitions(-DTARGET_LINUX)
elseif(BSD)
	add_definitions(-DTARGET_BSD)
elseif(APPLE)
	add_definitions(-DTARGET_APPLE)
elseif(CYGWIN)
	add_definitions(-DTARGET_CYGWIN)
else()
	if(ALLOW_UNKNOWN_TARGET)
		add_definitions(-DTARGET_UNKNOWN)
	else()
		message(FATAL_ERROR "Unknown Target System: \"" ${CMAKE_SYSTEM} "\"!")
	endif()
endif()
