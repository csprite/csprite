include(${CMAKE_CURRENT_SOURCE_DIR}/../common_defs.cmake)

set(BUILD_SHARED_LIBS   OFF CACHE INTERNAL "")
set(GLFW_VULKAN_STATIC  OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS    OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS     OFF CACHE INTERNAL "")
set(GLFW_INSTALL        OFF CACHE INTERNAL "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw/)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stb/)

file(
	GLOB_RECURSE THIRD_PARTY_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/tfd/tinyfiledialogs.c
)
add_library(third_party STATIC ${THIRD_PARTY_FILES})
target_include_directories(third_party PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tfd/)

target_link_libraries(third_party PUBLIC stb_static)
target_link_libraries(third_party PUBLIC imgui_static)
target_link_libraries(third_party PUBLIC glad_loader)
target_link_libraries(third_party PUBLIC glfw)
