include(${CMAKE_CURRENT_SOURCE_DIR}/../../common_defs.cmake)

add_library(stb_static STATIC ${CMAKE_CURRENT_SOURCE_DIR}/impl.c)
target_include_directories(stb_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
