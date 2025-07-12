#ifndef SRC_BASE_MATH_H
#define SRC_BASE_MATH_H

#include "base/types.h"

typedef struct {
	U32 w, h;
} Rect;

typedef struct {
	U32 min, max;
} Rng1DU32;

typedef struct {
	U64 min, max;
} Rng1DU64;

#define rect(_w, _h) (Rect){ .w = (_w), .h = (_h) }
#define rect_match(a, b) ((a).w == (b).w && (a).h == (b).h)
#define rect_area(r) ((r).w * (r).h)

#define rng2d_nil() ((Rng2D){0})
#define rng2d_xy_wh(_x, _y, _w, _h) ((Rng2D){ .min = { .x = (_x), .y = (_y) }, .max = { .x = (_x) + (_w), .y = (_y) + (_h) } })
#define rng2d_is_nil(r) ((r).min.x == 0 && (r).min.y == 0 && (r).max.x == 0 && (r).max.y == 0)

#define rng1_mag(r) ((r).max - (r).min)
#define rng1_is_mag_zero(r) (rng1_mag(r) == 0)

// Mathematical Operations
#define abs_s32(v) ((S32)labs(v))
#define abs_s64(v) ((S64)llabs(v))

#endif // SRC_BASE_MATH_H
