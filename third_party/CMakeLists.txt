include(${CMAKE_CURRENT_SOURCE_DIR}/../common_defs.cmake)

set(BUILD_SHARED_LIBS   OFF CACHE INTERNAL "build glfw as a shared library")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "build glfw examples")
set(GLFW_BUILD_TESTS    OFF CACHE INTERNAL "build glfw tests")
set(GLFW_BUILD_DOCS     OFF CACHE INTERNAL "build glfw docs")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw/)

file(
	GLOB_RECURSE THIRD_PARTY_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glad/glad.c
	${CMAKE_CURRENT_SOURCE_DIR}/tfd/tinyfiledialogs.c
)
add_library(third_party STATIC ${THIRD_PARTY_FILES})

target_include_directories(third_party PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/)
target_include_directories(third_party PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tfd/)
target_include_directories(third_party PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
target_include_directories(csprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(third_party PUBLIC glfw)
