#ifndef SRC_IMAGE_IMAGE_H_INCLUDED_
#define SRC_IMAGE_IMAGE_H_INCLUDED_
#pragma once

#include <stdint.h>
#include "gfx/gfx.h"

typedef struct {
	uint32_t width, height;
	pixel_t* pixels;
} image_t;

int Image_Create(image_t* image, uint32_t width, uint32_t height);
int Image_LoadFrom(image_t* image, const char* filePath);
void Image_Destroy(image_t* image);

#endif
