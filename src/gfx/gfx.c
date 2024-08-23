#include "gfx/gfx.h"
#include <stdlib.h>

void boundCheckDirty(Vec2_t start, Vec2_t end, const image_t* img, Rect_t* dirty) {
	dirty->start.x = start.x < 0 ? 0 : start.x;
	dirty->start.y = start.y < 0 ? 0 : start.y;
	dirty->end.x = end.x >= img->width ? img->width : end.x;
	dirty->end.y = end.y >= img->height ? img->height : end.y;
}

void calcDirty(const Rect_t* dirty, Rect_t* final, const image_t* img) {
	if (dirty->start.x < final->start.x) final->start.x = dirty->start.x;
	if (dirty->start.y < final->start.y) final->start.y = dirty->start.y;
	if (dirty->end.x > final->end.x) final->end.x = dirty->end.x;
	if (dirty->end.y > final->end.y) final->end.y = dirty->end.y;

	boundCheckDirty((Vec2_t){ final->start.x, final->start.y }, (Vec2_t){ final->end.x, final->end.y }, img, final);
}

/*
 Ensure Top Left & Bottom Right are correct coordinates, else swap variables
 */
void ensureRectCoords(Vec2_t* start, Vec2_t* end) {
	int t = 0;
	if (end->x < start->x) { t = end->x; end->x = start->x; start->x = t; }
	if (end->y < start->y) { t = end->y; end->y = start->y; start->y = t; }
}

Rect_t plotRect(Vec2_t start, Vec2_t end, image_t* img, pixel_t color) {
	Rect_t dirty = {0};

	ensureRectCoords(&start, &end);
	for (int y = start.y; y <= end.y; y++) {
		for (int x = start.x; x <= end.x; x++) {
			if (x > -1 && y > -1 && x < img->width && y < img->height) {
				img->pixels[(y * img->width) + x] = color;
			}
		}
	}

	boundCheckDirty(start, (Vec2_t){ end.x + 1, end.y + 1 }, img, &dirty);
	return dirty;
}

Rect_t plotEllipseRect(Vec2_t start, Vec2_t end, image_t* img, pixel_t color) {
	Rect_t dirty = {0};

	ensureRectCoords(&start, &end);
	boundCheckDirty(start, (Vec2_t){ end.x + 1, end.y + 1 }, img, &dirty);

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

Rect_t plotLine(Vec2_t start, Vec2_t end, image_t* img, pixel_t color) {
	Rect_t dirty = {0};

	int64_t dx  =  labs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
	int64_t dy  = -labs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
	int64_t err = dx + dy, e2;

	for (;;) {
		if (start.x > -1 && start.y > -1 && start.x < img->width && start.y < img->height) {
			img->pixels[(start.y * img->width) + start.x] = color;
			if (dirty.start.x > start.x) dirty.start.x = start.x;
			if (dirty.start.y > start.y) dirty.start.y = start.y;
			if (dirty.end.x < start.x) dirty.end.x = start.x;
			if (dirty.end.y < start.y) dirty.end.y = start.y;
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

	dirty.end.x++;
	dirty.end.y++;
	return dirty;
}
