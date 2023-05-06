file(
	GLOB_RECURSE THIRD_PARTY_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

add_library(third_party STATIC ${THIRD_PARTY_FILES})
include(${CMAKE_CURRENT_SOURCE_DIR}/../common_defs.txt)
target_include_directories(third_party PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
