file(
	GLOB_RECURSE THIRD_PARTY_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

add_library(third_party STATIC ${THIRD_PARTY_FILES})
target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
