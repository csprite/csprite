#ifndef CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#define CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#pragma once

#include "base/math.h"
#include "image/image.h"

void ensureRectCoords(Point* start, Point* end);

Rng2D plotRect(Point start, Point end, Image* img, Pixel color);
Rng2D plotEllipseRect(Point start, Point end, Image* img, Pixel color);
Rng2D plotLine(Point start, Point end, Image* img, Pixel color);

#endif
