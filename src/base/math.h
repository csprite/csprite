#ifndef SRC_BASE_MATH_H
#define SRC_BASE_MATH_H

#include "base/types.h"

typedef struct {
	S64 x, y;
} Point;

typedef struct {
	S64 w, h;
} Rect;

typedef struct {
	Point min; // Top-Left
	Point max; // Bottom-Right
} Rng2D;

typedef struct {
	U32 min, max;
} Rng1DU32;

typedef struct {
	U64 min, max;
} Rng1DU64;

#define abs_s64(v) ((S64)llabs(v))

#define Rng2D_Invalidate(r) ((r).min.x = (r).max.x = 0)
#define Rng2D_IsValid(r) ((r).min.x != (r).max.x)

#define rng1_mag(r) ((r).max - (r).min)
#define rng1_is_mag_zero(r) (rng1_mag(r) == 0)

B32 point_match(Point a, Point b);
B32 rect_match(Rect a, Rect b);
Rng2D rng2d_intersect(Rng2D a, Rng2D b);

#endif // SRC_BASE_MATH_H
