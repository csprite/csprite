#ifndef SRC_RASTER_MATH_H
#define SRC_RASTER_MATH_H 1
#pragma once

#include "base/types.h"
#include "base/math.h"

typedef struct {
	S32 x, y;
} Vec2S32;

typedef struct {
	U32 x, y;
} Vec2U32;

typedef struct {
	Vec2U32 min, max;
} Region;

// Ensure `min` is Top-Left & `max` is Bottom-Right
void v2s32_ensure_tl_br(Vec2S32* min, Vec2S32* max);
void v2u32_ensure_tl_br(Vec2U32* min, Vec2U32* max);

Rect   region_to_rect(Region r);
Region region_clipped(Rect bounds, Vec2S32 min, Vec2S32 max);
Region region_clipped_xy_wh(Rect bounds, Vec2S32 xy, Rect wh);

// Constructors & Utilities
#define v2_match(v1, v2) ((v1).x == (v2).x && (v1).y == (v2).y)

#define v2s32(_x, _y) ((Vec2S32){ .x = (S32)(_x), .y = (S32)(_y) })
#define v2u32(_x, _y) ((Vec2U32){ .x = (U32)(_x), .y = (U32)(_y) })

#define v2s32_to_v2u32(v) v2u32((v).x, (v).y)
#define v2u32_to_v2s32(v) v2s32((v).x, (v).y)

#define region_nil() ((Region){0})
#define region_xy_wh(_x, _y, _w, _h) ((Region){ .min = { .x = (_x), .y = (_y) }, .max = { .x = (_x) + (_w), .y = (_y) + (_h) } })
#define region_is_nil(r) ((r).min.x == 0 && (r).min.y == 0 && (r).max.x == 0 && (r).max.y == 0)

#endif // SRC_RASTER_MATH_H
