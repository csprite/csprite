#ifndef CSPRITE_SRC_IMAGE_IMAGE_H_INCLUDED_
#define CSPRITE_SRC_IMAGE_IMAGE_H_INCLUDED_
#pragma once

#include "base/types.h"

typedef struct {
	S64 width, height;
	Pixel* pixels;
} Image;

void Image_Init(Image* img, U32 width, U32 height);
S32 Image_InitFrom(Image* img, const char* filePath);
S32 Image_Write(Image* img, const char* filePath);
void Image_Deinit(Image* img);

#endif
