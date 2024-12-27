#ifndef CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#define CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#pragma once

#include "types.h"
#include "image/image.h"

void ensureRectCoords(Vec2* start, Vec2* end);

Rect plotRect(Vec2 start, Vec2 end, Image* img, Pixel color);
Rect plotEllipseRect(Vec2 start, Vec2 end, Image* img, Pixel color);
Rect plotLine(Vec2 start, Vec2 end, Image* img, Pixel color);

#endif
