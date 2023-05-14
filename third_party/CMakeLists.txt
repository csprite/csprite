file(
	GLOB_RECURSE THIRD_PARTY_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glad/glad.c
	${CMAKE_CURRENT_SOURCE_DIR}/tfd/tinyfiledialogs.c
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common_defs.cmake)
add_library(third_party STATIC ${THIRD_PARTY_FILES})

target_include_directories(third_party PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/)
target_include_directories(third_party PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tfd/)
target_include_directories(third_party PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
