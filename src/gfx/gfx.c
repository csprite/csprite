#include "gfx/gfx.h"
#include <math.h>

void set_pixel_unsafe(Bitmap* img, Point p, Pixel color) {
	img->pixels[(p.y * img->width) + p.x] = color;
}

void set_pixel(Bitmap* img, Point p, Pixel color) {
	if (p.x >= 0 && p.y >= 0 && p.x < (S64)img->width && p.y < (S64)img->height) {
		set_pixel_unsafe(img, p, color);
	}
}

void boundCheckDirty(Point start, Point end, const Bitmap* img, Rng2D* dirty) {
	dirty->min.x = start.x < 0 ? 0 : start.x;
	dirty->min.y = start.y < 0 ? 0 : start.y;
	dirty->max.x = end.x >= (S64)img->width ? img->width : (U64)end.x;
	dirty->max.y = end.y >= (S64)img->height ? img->height : (U64)end.y;
}

void calcDirty(const Rng2D* dirty, Rng2D* final, const Bitmap* img) {
	if (dirty->min.x < final->min.x) final->min.x = dirty->min.x;
	if (dirty->min.y < final->min.y) final->min.y = dirty->min.y;
	if (dirty->max.x > final->max.x) final->max.x = dirty->max.x;
	if (dirty->max.y > final->max.y) final->max.y = dirty->max.y;

	boundCheckDirty((Point){ final->min.x, final->min.y }, (Point){ final->max.x, final->max.y }, img, final);
}

void clip_rng2d_to_image_bounds(Rng2D* dirty, Rect bounds) {
	if (dirty->min.x < 0) dirty->min.x = 0;
	if (dirty->min.y < 0) dirty->min.y = 0;
	if (dirty->max.x >= bounds.w) dirty->max.x = bounds.w - 1;
	if (dirty->max.y >= bounds.h) dirty->max.y = bounds.h - 1;
}

// Often it's possible the points start bottom right & End at
// top left. This can be simply fixed by swapping the axes.
void _SwapAxesIfNeeded(Point* restrict start, Point* restrict end) {
	S32 t = 0;
	if (end->x < start->x) { t = end->x; end->x = start->x; start->x = t; }
	if (end->y < start->y) { t = end->y; end->y = start->y; start->y = t; }
}

Rng2D plotRect(Point start, Point end, Bitmap* img, Pixel color) {
	_SwapAxesIfNeeded(&start, &end);

	#pragma omp parallel
	{
		#pragma omp for
		for (S32 y = start.y; y <= end.y; y++) {
			for (S32 x = start.x; x <= end.x; x++) {
				set_pixel(img, point(x, y), color);
			}
		}
	}

	Rng2D dirty = { start, end };
	clip_rng2d_to_image_bounds(&dirty, rect(img->width, img->height));
	// NOTE(pegvin) - Update Dirty Handling Logic To
	// Be Inclusive So That There's No Need For +1
	dirty.max.x++;
	dirty.max.y++;
	return dirty;
}

Rng2D plotCircle(Point c, U32 r, B32 filled, Bitmap* img, Pixel color) {
	// Midpoint circle extends to r + 1, Hence drawing a circle of
	// radius 1 would not be 1x1 in dimension, but 2x2. So we just
	// decrement the radius internally.
	if (--r < 1) {
		img->pixels[(c.y * img->width) + c.x] = color;
		return rng2d_xy_wh(c.x, c.y, 1, 1);
	}

	for (S32 x = 0, y = -r; x < -y; x++) {
		// Simplifying:
		// > F64 yMid = y + 0.5;
		// > if ((x * x) + (yMid * yMid) > (r * r))
		// You can get rid of flops <https://godbolt.org/z/r3z6a6je8>
		if ((4 * x * x) + (4 * y * y) + (4 * y) + 1 > (S32)(4 * r * r)) {
			y++;
		}

		// Octants 1-8 Starting From Top Right Quadrant (I), Going Clockwise
		Point oct[8] = {
			{ c.x + x, c.y + y }, { c.x - y, c.y - x },
			{ c.x - y, c.y + x }, { c.x + x, c.y - y },
			{ c.x - x, c.y - y }, { c.x + y, c.y + x },
			{ c.x + y, c.y - x }, { c.x - x, c.y + y },
		};

		// NOTE(pegvin) - I am not sure if there's a better way to do
		// this than to bound check on each pixel (as done by `putPixel`)
		if (filled) {
			// Draw Lines From Boundary Of Left Octant To Boundary Of Right Octant
			for (U32 i = oct[7].x; i <= oct[0].x; i++) set_pixel(img, point(i, oct[7].y), color);
			for (U32 i = oct[6].x; i <= oct[1].x; i++) set_pixel(img, point(i, oct[6].y), color);
			for (U32 i = oct[5].x; i <= oct[2].x; i++) set_pixel(img, point(i, oct[5].y), color);
			for (U32 i = oct[4].x; i <= oct[3].x; i++) set_pixel(img, point(i, oct[4].y), color);
		} else {
			for EachIndex(i, 8) {
				set_pixel(img, oct[i], color);
			}
		}
	}

	Rng2D dirty = rng2d_xy_wh(c.x - r, c.y - r, (r * 2) + 1, (r * 2) + 1);
	clip_rng2d_to_image_bounds(&dirty, rect(img->width, img->height));

	// TODO(pegvin) - Currently due to how the texture function is written
	// It is not inclusive, i.e. It only updates till max.x - 1 & max.y - 1
	// Hence the need for incrementing by 1. This should be fixed, As Rng2D
	// represents a region, hence max.x & max.y values should be inclusive.
	dirty.max.x++;
	dirty.max.y++;
	return dirty;
}

Rng2D plotEllipseRect(Point start, Point end, Bitmap* img, Pixel color) {
	Rng2D dirty = rng2d_nil();

	_SwapAxesIfNeeded(&start, &end);
	boundCheckDirty(start, (Point){ end.x + 1, end.y + 1 }, img, &dirty);

	S64 a = abs_s64(end.x - start.x), b = abs_s64(end.y - start.y), b1 = b & 1;
	S64 dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
	S64 err = dx + dy + b1 * a * a, e2;

	if (start.x > end.x) { start.x = end.x; end.x += a; }
	if (start.y > end.y) start.y = end.y;
	start.y += (b + 1) / 2; end.y = start.y - b1;
	a *= 8 * a; b1 = 8 * b * b;

	do {
		set_pixel(img, point(end.x, start.y), color);
		set_pixel(img, point(start.x, start.y), color);
		set_pixel(img, point(start.x, end.y), color);
		set_pixel(img, point(end.x, end.y), color);

		e2 = 2 * err;
		if (e2 <= dy) { start.y++; end.y--; err += dy += a; }
		if (e2 >= dx || 2 * err > dy) { start.x++; end.x--; err += dx += b1; }
	} while (start.x <= end.x);

	for (; start.y - end.y < b; start.y++, end.y--) {
		set_pixel(img, point(start.x - 1, start.y), color);
		set_pixel(img, point(start.x + 1, start.y), color);
		set_pixel(img, point(start.x - 1, end.y), color);
		set_pixel(img, point(end.x + 1, end.y), color);
	}

	return dirty;
}

Rng2D plotLine(Point start, Point end, Bitmap* img, Pixel color) {
	Rng2D dirty = rng2d_nil();

	S64 dx  =  abs_s64(end.x - start.x), sx = start.x < end.x ? 1 : -1;
	S64 dy  = -abs_s64(end.y - start.y), sy = start.y < end.y ? 1 : -1;
	S64 err = dx + dy, e2;

	for (;start.x != end.x || start.y != end.y;) {
		set_pixel(img, start, color);

		if (dirty.min.x > start.x) dirty.min.x = start.x;
		else if (dirty.max.x < start.x) dirty.max.x = start.x;
		if (dirty.min.y > start.y) dirty.min.y = start.y;
		else if (dirty.max.y < start.y) dirty.max.y = start.y;

		e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			start.x += sx;
		}
		if (e2 <= dx) {
			err += dx;
			start.y += sy;
		}
	}

	dirty.max.x++;
	dirty.max.y++;
	return dirty;
}
