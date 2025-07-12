#include "raster/gfx.h"

void set_pixel_unsafe(Raster* img, Vec2U32 p, RGBAU8 color) {
	img->data[(p.y * img->dim.w) + p.x] = color;
}

void set_pixel(Raster* img, Vec2S32 p, RGBAU8 color) {
	if (p.x >= 0 && p.y >= 0 && p.x < (S32)img->dim.w && p.y < (S32)img->dim.h) {
		set_pixel_unsafe(img, v2s32_to_v2u32(p), color);
	}
}

Region rs_gfx_draw_rect(Raster* r, RGBAU8 color, Vec2S32 start, Vec2S32 end) {
	Region dirty = region_clipped(r->dim, start, end);
	Rect dirty_rect = region_to_rect(dirty);

	#pragma omp parallel for
	for EachIndex(i, rect_area(dirty_rect)) {
		Vec2U32 v = v2u32((i % dirty_rect.w) + dirty.min.x, (i / dirty_rect.w) + dirty.min.y);
		set_pixel_unsafe(r, v, color);
	}

	return dirty;
}

Region rs_gfx_draw_line(Raster* r, RGBAU8 color, Vec2S32 start, Vec2S32 end) {
	Region dirty = region_clipped(r->dim, start, end);

	S32 dx  =  abs_s32(end.x - start.x), sx = start.x < end.x ? 1 : -1;
	S32 dy  = -abs_s32(end.y - start.y), sy = start.y < end.y ? 1 : -1;
	S32 err = dx + dy, e2;

	for (;start.x != end.x || start.y != end.y;) {
		set_pixel(r, start, color);

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

	return dirty;
}

Region rs_gfx_draw_circle(Raster* rs, RGBAU8 color, Vec2S32 c, U32 r, B32 is_filled) {
	// Midpoint circle extends to r + 1, Hence drawing a circle of
	// radius 1 would not be 1x1 in dimension, but 2x2. So we just
	// decrement the radius internally.
	if (--r < 1) {
		rs->data[(c.y * rs->dim.w) + c.x] = color;
		return region_xy_wh(c.x, c.y, 1, 1);
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
		Vec2S32 oct[8] = {
			{ c.x + x, c.y + y }, { c.x - y, c.y - x },
			{ c.x - y, c.y + x }, { c.x + x, c.y - y },
			{ c.x - x, c.y - y }, { c.x + y, c.y + x },
			{ c.x + y, c.y - x }, { c.x - x, c.y + y },
		};

		// NOTE(pegvin) - I am not sure if there's a better way to do
		// this than to bound check on each pixel (as done by `putRGBAU8`)
		if (is_filled) {
			// Draw Lines From Boundary Of Left Octant To Boundary Of Right Octant
			for (S32 i = oct[7].x; i <= oct[0].x; i++) set_pixel(rs, v2s32(i, oct[7].y), color);
			for (S32 i = oct[6].x; i <= oct[1].x; i++) set_pixel(rs, v2s32(i, oct[6].y), color);
			for (S32 i = oct[5].x; i <= oct[2].x; i++) set_pixel(rs, v2s32(i, oct[5].y), color);
			for (S32 i = oct[4].x; i <= oct[3].x; i++) set_pixel(rs, v2s32(i, oct[4].y), color);
		} else {
			for EachIndex(i, 8) {
				set_pixel(rs, oct[i], color);
			}
		}
	}

	return region_clipped_xy_wh(
		rs->dim,
		v2s32(c.x - r, c.y - r),
		rect(r * 2, r * 2)
	);
}

Region rs_gfx_draw_ellipse(Raster* r, RGBAU8 color, Vec2S32 start, Vec2S32 end) {
	Region dirty = region_clipped(r->dim, start, end);

	S64 a = abs_s64(end.x - start.x), b = abs_s64(end.y - start.y), b1 = b & 1;
	S64 dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
	S64 err = dx + dy + b1 * a * a, e2;

	if (start.x > end.x) { start.x = end.x; end.x += a; }
	if (start.y > end.y) start.y = end.y;
	start.y += (b + 1) / 2; end.y = start.y - b1;
	a *= 8 * a; b1 = 8 * b * b;

	do {
		set_pixel(r, v2s32(end.x, start.y), color);
		set_pixel(r, v2s32(start.x, start.y), color);
		set_pixel(r, v2s32(start.x, end.y), color);
		set_pixel(r, v2s32(end.x, end.y), color);

		e2 = 2 * err;
		if (e2 <= dy) { start.y++; end.y--; err += dy += a; }
		if (e2 >= dx || 2 * err > dy) { start.x++; end.x--; err += dx += b1; }
	} while (start.x <= end.x);

	for (; start.y - end.y < b; start.y++, end.y--) {
		set_pixel(r, v2s32(start.x - 1, start.y), color);
		set_pixel(r, v2s32(start.x + 1, start.y), color);
		set_pixel(r, v2s32(start.x - 1, end.y), color);
		set_pixel(r, v2s32(end.x + 1, end.y), color);
	}

	return dirty;
}
