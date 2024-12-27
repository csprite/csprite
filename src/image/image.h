#ifndef CSPRITE_SRC_IMAGE_IMAGE_H_INCLUDED_
#define CSPRITE_SRC_IMAGE_IMAGE_H_INCLUDED_
#pragma once

#include <stdint.h>
#include "pixel.h"

typedef struct {
	int64_t width, height;
	Pixel* pixels;
} Image;

void image_init(Image* img, uint32_t width, uint32_t height);
int image_initFrom(Image* img, const char* filePath);
int image_write(Image* img, const char* filePath);
void image_deinit(Image* img);

#endif
