#ifndef CSPRITE_SRC_IMAGE_IMAGE_H_INCLUDED_
#define CSPRITE_SRC_IMAGE_IMAGE_H_INCLUDED_
#pragma once

#include <stdint.h>
#include "pixel.h"

typedef struct {
	int64_t width, height;
	pixel_t* pixels;
} image_t;

void image_init(image_t* img, uint32_t width, uint32_t height);
int image_initFrom(image_t* img, const char* filePath);
int image_write(image_t* img, const char* filePath);
void image_deinit(image_t* img);

#endif
