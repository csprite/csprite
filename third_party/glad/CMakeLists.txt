include(${CMAKE_CURRENT_SOURCE_DIR}/../../common_defs.cmake)

add_library(glad_loader STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad.c)
target_include_directories(glad_loader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

