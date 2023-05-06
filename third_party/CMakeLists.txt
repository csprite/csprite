file(
	GLOB_RECURSE THIRD_PARTY_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgooeystyles/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/FileBrowser/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ini/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/log/*.c
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common_defs.txt)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/zlib)
add_library(third_party STATIC ${THIRD_PARTY_FILES})
target_link_libraries(third_party PUBLIC zlibstatic)

target_include_directories(third_party PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
