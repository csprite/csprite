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

#define Rng2D_Invalidate(r) ((r).min.x = (r).max.x = 0)
#define Rng2D_IsValid(r) ((r).min.x != (r).max.x)

B32 point_match(Point a, Point b);
B32 rect_match(Rect a, Rect b);
Rng2D rng2d_intersect(Rng2D a, Rng2D b);

#endif // SRC_BASE_MATH_H
