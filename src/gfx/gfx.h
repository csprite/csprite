#ifndef CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#define CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#pragma once

#include "types.h"
#include "image/image.h"

void ensureRectCoords(int* x0, int* y0, int* x1, int* y1);

mmRect_t plotRect(int x0, int y0, int x1, int y1, image_t* img, pixel_t color);
mmRect_t plotEllipseRect(int x0, int y0, int x1, int y1, image_t* img, pixel_t color);
mmRect_t plotLine(int x0, int y0, int x1, int y1, image_t* img, pixel_t color);

#endif
