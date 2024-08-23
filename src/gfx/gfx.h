#ifndef CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#define CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#pragma once

#include "types.h"
#include "image/image.h"

void ensureRectCoords(Vec2_t* start, Vec2_t* end);

Rect_t plotRect(Vec2_t start, Vec2_t end, image_t* img, pixel_t color);
Rect_t plotEllipseRect(Vec2_t start, Vec2_t end, image_t* img, pixel_t color);
Rect_t plotLine(Vec2_t start, Vec2_t end, image_t* img, pixel_t color);

#endif
