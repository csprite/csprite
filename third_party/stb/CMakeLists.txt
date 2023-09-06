include(${CMAKE_CURRENT_SOURCE_DIR}/../../common_defs.cmake)

# only keep png, jpeg, bmp, psd and tga support
add_compile_definitions(STBI_NO_GIF)
add_compile_definitions(STBI_NO_HDR)
add_compile_definitions(STBI_NO_PIC)
add_compile_definitions(STBI_NO_PNM)

add_library(stb_static STATIC ${CMAKE_CURRENT_SOURCE_DIR}/impl.c)
target_include_directories(stb_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
