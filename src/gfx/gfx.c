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

mmRect_t plotRect(int x0, int y0, int x1, int y1, image_t* img, pixel_t color) {
	mmRect_t dirty = { img->width, img->height, 0, 0 };

	ensureRectCoords(&x0, &y0, &x1, &y1);
	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			if (x > -1 && y > -1 && x < img->width && y < img->height) {
				img->pixels[(y * img->width) + x] = color;
			}
		}
	}

	boundCheckDirty(x0, y0, x1 + 1, y1 + 1, img, &dirty);
	return dirty;
}

mmRect_t plotEllipseRect(int x0, int y0, int x1, int y1, image_t* img, pixel_t color) {
	mmRect_t dirty = { img->width, img->height, 0, 0 };

	ensureRectCoords(&x0, &y0, &x1, &y1);
	boundCheckDirty(x0, y0, x1 + 1, y1 + 1, img, &dirty);

	long long a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1;
	long long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
	long long err = dx + dy + b1 * a * a, e2;

	if (x0 > x1) { x0 = x1; x1 += a; }
	if (y0 > y1) y0 = y1;
	y0 += (b + 1) / 2; y1 = y0 - b1;
	a *= 8 * a; b1 = 8 * b * b;

	do {
		if (x1 > -1 && y0 > -1 && x1 < img->width && y0 < img->height) {
			img->pixels[(y0 * img->width) + x1] = color;
		}
		if (x0 > -1 && y0 > -1 && x0 < img->width && y0 < img->height) {
			img->pixels[(y0 * img->width) + x0] = color;
		}
		if (x0 > -1 && y1 > -1 && x0 < img->width && y1 < img->height) {
			img->pixels[(y1 * img->width) + x0] = color;
		}
		if (x1 > -1 && y1 > -1 && x1 < img->width && y1 < img->height) {
			img->pixels[(y1 * img->width) + x1] = color;
		}

		e2 = 2 * err;
		if (e2 <= dy) { y0++; y1--; err += dy += a; }
		if (e2 >= dx || 2 * err > dy) { x0++; x1--; err += dx += b1; }
	} while (x0 <= x1);

	while (y0-y1 < b) {
		if (x0-1 > -1 && y0 > -1 && x0-1 < img->width && y0 < img->height) {
			img->pixels[(y0 * img->width) + x0-1] = color;
		}
		if (x1+1 > -1 && y0 > -1 && x1+1 < img->width && y0 < img->height) {
			img->pixels[(y0 * img->width) + x0+1] = color;
		}
		y0++;
		if (x0-1 > -1 && y1 > -1 && x0-1 < img->width && y1 < img->height) {
			img->pixels[(y1 * img->width) + x0-1] = color;
		}
		if (x1+1 > -1 && y1 > -1 && x1+1 < img->width && y1 < img->height) {
			img->pixels[(y1 * img->width) + x1+1] = color;
		}
		y1--;
	}

	return dirty;
}

mmRect_t plotLine(int x0, int y0, int x1, int y1, image_t* img, pixel_t color) {
	mmRect_t dirty = { img->width, img->height, 0, 0 };

	long long dx  =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	long long dy  = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	long long err = dx + dy, e2;

	for (;;) {
		if (x0 > -1 && y0 > -1 && x0 < img->width && y0 < img->height) {
			img->pixels[(y0 * img->width) + x0] = color;
			if (dirty.min_x > x0) dirty.min_x = x0;
			if (dirty.min_y > y0) dirty.min_y = y0;
			if (dirty.max_x < x0) dirty.max_x = x0;
			if (dirty.max_y < y0) dirty.max_y = y0;
		}
		if (x0 == x1 && y0 == y1) {
			break;
		}

		e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}

	dirty.max_x++;
	dirty.max_y++;
	return dirty;
}
