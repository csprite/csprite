set(SKIP_INSTALL_ALL ON  CACHE INTERNAL "")

set(ZLIB_FOUND)
set(ZLIB_LIBRARY zlibstatic)
set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
set(ZLIB_INCLUDE_DIRS ${ZLIB_DIR} ${CMAKE_BINARY_DIR}/third_party/zlib) # Zlib generated zconf.h file
set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS} CACHE PATH "")
include_directories(${ZLIB_INCLUDE_DIRS})

set(PNG_FOUND ON)
set(PNG_LIBRARY png_static)
set(PNG_LIBRARIES ${PNG_LIBRARY})
set(PNG_INCLUDE_DIRS ${LIBPNG_DIR} ${CMAKE_CURRENT_BINARY_DIR}/third_party/libpng) # Libpng generated pnglibconf.h file
set(PNG_INCLUDE_DIR ${PNG_INCLUDE_DIRS} CACHE PATH "")
set(PNG_PNG_INCLUDE_DIR ${PNG_INCLUDE_DIRS} CACHE PATH "")
include_directories(${PNG_INCLUDE_DIRS})
add_definitions(-DPNG_NO_MMX_CODE)

set(PNG_STATIC                 ON  CACHE BOOL "")
set(PNG_HARDWARE_OPTIMIZATIONS ON  CACHE BOOL "")
set(PNG_SHARED                 OFF CACHE BOOL "")
set(PNG_TESTS                  OFF CACHE BOOL "")
set(PNG_FRAMEWORK              OFF CACHE BOOL "")
set(PNG_EXECUTABLES            OFF CACHE BOOL "")
set(PNG_DEBUG                  OFF CACHE BOOL "")
set(PNG_BUILD_ZLIB             OFF CACHE BOOL "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libpng/)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/zlib/)
target_link_libraries(csprite PUBLIC zlibstatic)

set(PNG_STATIC                 ON  CACHE INTERNAL "")
set(PNG_HARDWARE_OPTIMIZATIONS ON  CACHE INTERNAL "")
set(PNG_SHARED                 OFF CACHE INTERNAL "")
set(PNG_FRAMEWORK              OFF CACHE INTERNAL "")
set(PNG_EXECUTABLES            OFF CACHE INTERNAL "")
set(PNG_TESTS                  OFF CACHE INTERNAL "")
set(PNG_SHARED                 OFF CACHE INTERNAL "")
set(PNG_DEBUG                  OFF CACHE INTERNAL "")

