cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(ENABLE_APPLE_BUNDLE "Build Apple Bundle, Used For Releases & Is Ignored On Non-Apple Target" OFF)

project(csprite C CXX)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.c)

# On Windows, Enable RC Language & Add It To Sources
if(WIN32)
	enable_language(RC)
	set(SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/data/windows.rc)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/common_defs.cmake)

if(APPLE AND BUILD_APPLE_BUNDLE)
	add_executable(csprite MACOSX_BUNDLE ${SRC_FILES})
	set_source_files_properties(
		${CMAKE_CURRENT_SOURCE_DIR}/csprite.icns PROPERTIES
		MACOSX_PACKAGE_LOCATION "Resources"
	)
	set_target_properties(
		csprite PROPERTIES BUNDLE True
		MACOSX_BUNDLE_BUNDLE_NAME "csprite"                  # CFBundleName
		MACOSX_BUNDLE_BUNDLE_VERSION "0.0.1"                 # CFBundleVersion
		MACOSX_BUNDLE_LONG_VERSION_STRING "0.0.1"            # CFBundleLongVersionString
		MACOSX_BUNDLE_SHORT_VERSION_STRING "0.0.1"           # CFBundleShortVersionString
		MACOSX_BUNDLE_COPYRIGHT "adityaraj"                  # NSHumanReadableCopyright
		MACOSX_BUNDLE_GUI_IDENTIFIER "pegvin.csprite.github" # CFBundleIdentifier
		MACOSX_BUNDLE_ICON_FILE "csprite.icns"               # CFBundleIconFile
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/data/Info.plist
	)
else()
	add_executable(csprite ${SRC_FILES})
endif()

set(JSON_BuildTests                     OFF CACHE INTERNAL "")
set(JSON_CI                             OFF CACHE INTERNAL "")
set(JSON_Diagnostics                    OFF CACHE INTERNAL "")
set(JSON_GlobalUDLs                     ON  CACHE INTERNAL "")
set(JSON_ImplicitConversions            ON  CACHE INTERNAL "")
set(JSON_DisableEnumSerialization       OFF CACHE INTERNAL "")
set(JSON_LegacyDiscardedValueComparison OFF CACHE INTERNAL "")
set(JSON_Install                        OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders                OFF CACHE INTERNAL "")
set(JSON_SystemInclude                  OFF CACHE INTERNAL "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/)
target_link_libraries(csprite PUBLIC nlohmann_json::nlohmann_json)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/imbase/)
target_link_libraries(csprite PUBLIC imbase)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb/)
target_link_libraries(csprite PUBLIC stb_static)

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

