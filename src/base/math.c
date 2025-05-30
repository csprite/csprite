#include "base/math.h"

B32 point_match(Point a, Point b) {
	return a.x == b.x && a.y == b.y;
}

B32 rect_match(Rect a, Rect b) {
	return a.w == b.w && a.h == b.h;
}

Rng2D rng2d_intersect(Rng2D a, Rng2D b) {
	Rng2D c;
	c.min.x = Max(a.min.x, b.min.x);
	c.min.y = Max(a.min.y, b.min.y);
	c.max.x = Min(a.max.x, b.max.x);
	c.max.y = Min(a.max.y, b.max.y);
	return c;
}
