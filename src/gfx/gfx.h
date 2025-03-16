#ifndef CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#define CSPRITE_SRC_GFX_GFX_H_INCLUDED_
#pragma once

#include "base/types.h"
#include "image/image.h"

void ensureRectCoords(Point* start, Point* end);

Rect plotRect(Point start, Point end, Image* img, Pixel color);
Rect plotEllipseRect(Point start, Point end, Image* img, Pixel color);
Rect plotLine(Point start, Point end, Image* img, Pixel color);

#endif
