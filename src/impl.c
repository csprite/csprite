// impl.c is a separate file which contains implementation for multiple "header only" libraries.
// this was done to reduce compile times, & compiling same thing over and over again.

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define UTILS_IMPLEMENTATION 1
#include "utils.h"
