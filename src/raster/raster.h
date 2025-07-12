#ifndef SRC_RASTER_RASTER_H
#define SRC_RASTER_RASTER_H 1
#pragma once

#include "base/types.h"
#include "base/math.h"
#include "base/arena.h"
#include "base/string.h"

typedef struct {
	U8 r, g, b, a;
} RGBAU8;

typedef struct {
	Rect dim;
	RGBAU8* data;
} Raster;

Raster rs_init(Arena* a, Rect dim);
Raster rs_init_from_file(Arena* a, String8 filepath);

#define rs_is_invalid(r) (rect_area(r.dim) == 0)

#endif // SRC_RASTER_RASTER_H
