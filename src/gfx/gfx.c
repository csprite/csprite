#include "gfx/gfx.h"
#include <stdlib.h>

void boundCheckDirty(int x0, int y0, int x1, int y1, const image_t* img, mmRect_t* dirty) {
	dirty->min_x = x0 < 0 ? 0 : x0;
	dirty->min_y = y0 < 0 ? 0 : y0;
	dirty->max_x = x1 >= img->width ? img->width : x1;
	dirty->max_y = y1 >= img->height ? img->height : y1;
}

void calcDirty(const mmRect_t* dirty, mmRect_t* final, const image_t* img) {
	if (dirty->min_x < final->min_x) final->min_x = dirty->min_x;
	if (dirty->min_y < final->min_y) final->min_y = dirty->min_y;
	if (dirty->max_x > final->max_x) final->max_x = dirty->max_x;
	if (dirty->max_y > final->max_y) final->max_y = dirty->max_y;

	boundCheckDirty(final->min_x, final->min_y, final->max_x, final->max_y, img, final);
}

/*
 x0, y0 -> Top Left
 x1, y1 -> Bottom Right
 Ensure Top Left & Bottom Right are correct coordinates, else swap variables
 */
void ensureRectCoords(int* x0, int* y0, int* x1, int* y1) {
	int t = 0;
	if (*x1 < *x0) { t = *x1; *x1 = *x0; *x0 = t; }
	if (*y1 < *y0) { t = *y1; *y1 = *y0; *y0 = t; }
}

mmRect_t plotRect(Vec2_t start, Vec2_t end, image_t* img, pixel_t color) {
	mmRect_t dirty = { img->width, img->height, 0, 0 };

	ensureRectCoords(&start.x, &start.y, &end.x, &end.y);
	for (int y = start.y; y <= end.y; y++) {
		for (int x = start.x; x <= end.x; x++) {
			if (x > -1 && y > -1 && x < img->width && y < img->height) {
				img->pixels[(y * img->width) + x] = color;
			}
		}
	}

	boundCheckDirty(start.x, start.y, end.x + 1, end.y + 1, img, &dirty);
	return dirty;
}

mmRect_t plotEllipseRect(Vec2_t start, Vec2_t end, image_t* img, pixel_t color) {
	mmRect_t dirty = { img->width, img->height, 0, 0 };

	ensureRectCoords(&start.x, &start.y, &end.x, &end.y);
	boundCheckDirty(start.x, start.y, end.x + 1, end.y + 1, img, &dirty);

	int64_t a = abs(end.x - start.x), b = abs(end.y - start.y), b1 = b & 1;
	int64_t dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
	int64_t err = dx + dy + b1 * a * a, e2;

	if (start.x > end.x) { start.x = end.x; end.x += a; }
	if (start.y > end.y) start.y = end.y;
	start.y += (b + 1) / 2; end.y = start.y - b1;
	a *= 8 * a; b1 = 8 * b * b;

	do {
		if (end.x > -1 && start.y > -1 && end.x < img->width && start.y < img->height) {
			img->pixels[(start.y * img->width) + end.x] = color;
		}
		if (start.x > -1 && start.y > -1 && start.x < img->width && start.y < img->height) {
			img->pixels[(start.y * img->width) + start.x] = color;
		}
		if (start.x > -1 && end.y > -1 && start.x < img->width && end.y < img->height) {
			img->pixels[(end.y * img->width) + start.x] = color;
		}
		if (end.x > -1 && end.y > -1 && end.x < img->width && end.y < img->height) {
			img->pixels[(end.y * img->width) + end.x] = color;
		}

		e2 = 2 * err;
		if (e2 <= dy) { start.y++; end.y--; err += dy += a; }
		if (e2 >= dx || 2 * err > dy) { start.x++; end.x--; err += dx += b1; }
	} while (start.x <= end.x);

	while (start.y-end.y < b) {
		if (start.x-1 > -1 && start.y > -1 && start.x-1 < img->width && start.y < img->height) {
			img->pixels[(start.y * img->width) + start.x-1] = color;
		}
		if (end.x+1 > -1 && start.y > -1 && end.x+1 < img->width && start.y < img->height) {
			img->pixels[(start.y * img->width) + start.x+1] = color;
		}
		start.y++;
		if (start.x-1 > -1 && end.y > -1 && start.x-1 < img->width && end.y < img->height) {
			img->pixels[(end.y * img->width) + start.x-1] = color;
		}
		if (end.x+1 > -1 && end.y > -1 && end.x+1 < img->width && end.y < img->height) {
			img->pixels[(end.y * img->width) + end.x+1] = color;
		}
		end.y--;
	}

	return dirty;
}

mmRect_t plotLine(Vec2_t start, Vec2_t end, image_t* img, pixel_t color) {
	mmRect_t dirty = { img->width, img->height, 0, 0 };

	int64_t dx  =  labs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
	int64_t dy  = -labs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
	int64_t err = dx + dy, e2;

	for (;;) {
		if (start.x > -1 && start.y > -1 && start.x < img->width && start.y < img->height) {
			img->pixels[(start.y * img->width) + start.x] = color;
			if (dirty.min_x > start.x) dirty.min_x = start.x;
			if (dirty.min_y > start.y) dirty.min_y = start.y;
			if (dirty.max_x < start.x) dirty.max_x = start.x;
			if (dirty.max_y < start.y) dirty.max_y = start.y;
		}
		if (start.x == end.x && start.y == end.y) {
			break;
		}

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

	dirty.max_x++;
	dirty.max_y++;
	return dirty;
}
