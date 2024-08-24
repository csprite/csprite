#ifndef CSPRITE_SRC_IMAGE_IMAGE_H_INCLUDED_
#define CSPRITE_SRC_IMAGE_IMAGE_H_INCLUDED_
#pragma once

#include <stdint.h>
#include "pixel.h"

typedef struct {
	int64_t width, height;
	pixel_t* pixels;
} image_t;

int ImageInit(image_t* img, uint32_t width, uint32_t height);
int ImageInitFrom(image_t* img, const char* filePath);
int ImageWriteTo(image_t* img, const char* filePath);
void ImageDestroy(image_t* img);

#endif
