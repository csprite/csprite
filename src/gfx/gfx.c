#include "gfx/gfx.h"
#include <stdlib.h>

void boundCheckDirty(Point start, Point end, const Image* img, Rng2D* dirty) {
	dirty->min.x = start.x < 0 ? 0 : start.x;
	dirty->min.y = start.y < 0 ? 0 : start.y;
	dirty->max.x = end.x >= img->width ? img->width : end.x;
	dirty->max.y = end.y >= img->height ? img->height : end.y;
}

void calcDirty(const Rng2D* dirty, Rng2D* final, const Image* img) {
	if (dirty->min.x < final->min.x) final->min.x = dirty->min.x;
	if (dirty->min.y < final->min.y) final->min.y = dirty->min.y;
	if (dirty->max.x > final->max.x) final->max.x = dirty->max.x;
	if (dirty->max.y > final->max.y) final->max.y = dirty->max.y;

	boundCheckDirty((Point){ final->min.x, final->min.y }, (Point){ final->max.x, final->max.y }, img, final);
}

/*
 Ensure Top Left & Bottom Right are correct coordinates, else swap variables
 */
void ensureRectCoords(Point* start, Point* end) {
	S32 t = 0;
	if (end->x < start->x) { t = end->x; end->x = start->x; start->x = t; }
	if (end->y < start->y) { t = end->y; end->y = start->y; start->y = t; }
}

Rng2D plotRect(Point start, Point end, Image* img, Pixel color) {
	Rng2D dirty = {0};

	ensureRectCoords(&start, &end);
	for (S32 y = start.y; y <= end.y; y++) {
		for (S32 x = start.x; x <= end.x; x++) {
			if (x > -1 && y > -1 && x < img->width && y < img->height) {
				img->pixels[(y * img->width) + x] = color;
			}
		}
	}

	boundCheckDirty(start, (Point){ end.x + 1, end.y + 1 }, img, &dirty);
	return dirty;
}

Rng2D plotEllipseRect(Point start, Point end, Image* img, Pixel color) {
	Rng2D dirty = {0};

	ensureRectCoords(&start, &end);
	boundCheckDirty(start, (Point){ end.x + 1, end.y + 1 }, img, &dirty);

	S64 a = abs(end.x - start.x), b = abs(end.y - start.y), b1 = b & 1;
	S64 dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
	S64 err = dx + dy + b1 * a * a, e2;

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

Rng2D plotLine(Point start, Point end, Image* img, Pixel color) {
	Rng2D dirty = {0};

	S64 dx  =  labs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
	S64 dy  = -labs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
	S64 err = dx + dy, e2;

	for (;;) {
		if (start.x > -1 && start.y > -1 && start.x < img->width && start.y < img->height) {
			img->pixels[(start.y * img->width) + start.x] = color;
			if (dirty.min.x > start.x) dirty.min.x = start.x;
			if (dirty.min.y > start.y) dirty.min.y = start.y;
			if (dirty.max.x < start.x) dirty.max.x = start.x;
			if (dirty.max.y < start.y) dirty.max.y = start.y;
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

	dirty.max.x++;
	dirty.max.y++;
	return dirty;
}
