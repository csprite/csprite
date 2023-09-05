#include "stb_image.h"

#include "../utils.h"
#include "../macros.h"

#include "xy_stack.h"
#include "tools/tools.hpp"

BrushShape_t BrushShape = BRUSH_SHAPE_CIRCLE;
int32_t BrushSize = 1;

int32_t Tools_GetBrushSize() {
	return BrushSize;
}

BrushShape_t Tools_GetBrushShape() {
	return BrushShape;
}

void Tools_SetBrushShape(BrushShape_t _bshape) {
	BrushShape = _bshape;
}

void Tools_SetBrushSize(int32_t NewBrushSize) {
	BrushSize = NewBrushSize;
}

bool Tool_Brush(Pixel* Pixels, Pixel& Color, uint32_t st_x, uint32_t st_y, uint32_t w, uint32_t h) {
	bool didChange = false;
	// dirY = direction Y
	// dirX = direction X

	// Loops From -BrushSize/2 To BrushSize/2, ex: -6/2 to 6/2 -> -3 to 3
	int x = 0, y = 0;
	for (int dirY = -BrushSize / 2; dirY < BrushSize / 2 + 1; dirY++) {
		for (int dirX = -BrushSize / 2; dirX < BrushSize / 2 + 1; dirX++) {
			if (st_x + dirX < 0 || st_x + dirX >= w || st_y + dirY < 0 || st_y + dirY > h)
				continue;

			if (BrushShape == BRUSH_SHAPE_CIRCLE && dirX * dirX + dirY * dirY > BrushSize / 2 * BrushSize / 2)
				continue;

			x = st_x + dirX;
			y = st_y + dirY;
			if (!(x >= 0 && y >= 0 && x < w && y < h))
				continue;

			Pixel& pixel = Pixels[((st_y + dirY) * w) + (st_x + dirX)];
			pixel = Color;
			didChange = true;
		}
	}
	return didChange;
}

bool Tool_Line(Pixel* Pixels, Pixel& Color, int x0, int y0, int x1, int y1, uint32_t w, uint32_t h) {
	int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */
	bool didChange = false;

	for (;;) {
		didChange = Tool_Brush(Pixels, Color, x0, y0, w, h) || didChange;
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
	return didChange;
}

/*
 In Simplest form a rectangle is made up of 4 lines,
 this is how we make our rectangle using 2 x, y co-ords.
 Since we're using the drawLine Function for making our,
 rectangle we don't need to worry about round edges.
 x0, y0           x1, y0
   .------->--------.
   |                |
   |                |
   ^                v
   |                |
   |                |
   .-------<--------.
 x0, y1           x1, y1
*/

bool Tool_Rect(Pixel* Pixels, Pixel& Color, int x0, int y0, int x1, int y1, uint32_t w, uint32_t h) {
	bool didChange = false;
	didChange = Tool_Line(Pixels, Color, x0, y0, x1, y0, w, h) || didChange;
	didChange = Tool_Line(Pixels, Color, x1, y0, x1, y1, w, h) || didChange;
	didChange = Tool_Line(Pixels, Color, x1, y1, x0, y1, w, h) || didChange;
	didChange = Tool_Line(Pixels, Color, x0, y1, x0, y0, w, h) || didChange;
	return didChange;
}

bool Tool_Circle(Pixel* Pixels, Pixel& Color, int centreX, int centreY, int radius, uint32_t w, uint32_t h) {
	const int diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);
	bool didChange = false;

	while (x >= y) {
		// Each of the following renders an octant of the circle
		didChange = Tool_Brush(Pixels, Color, centreX + x, centreY - y, w, h) || didChange;
		didChange = Tool_Brush(Pixels, Color, centreX + x, centreY + y, w, h) || didChange;
		didChange = Tool_Brush(Pixels, Color, centreX - x, centreY - y, w, h) || didChange;
		didChange = Tool_Brush(Pixels, Color, centreX - x, centreY + y, w, h) || didChange;
		didChange = Tool_Brush(Pixels, Color, centreX + y, centreY - x, w, h) || didChange;
		didChange = Tool_Brush(Pixels, Color, centreX + y, centreY + x, w, h) || didChange;
		didChange = Tool_Brush(Pixels, Color, centreX - y, centreY - x, w, h) || didChange;
		didChange = Tool_Brush(Pixels, Color, centreX - y, centreY + x, w, h) || didChange;

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}
		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
	return didChange;
}

xy_stack_t* floodFillLocStack = NULL;

bool Tool_FloodFill(
	Pixel* Pixels,
	Pixel& OldColor,
	Pixel& NewColor,
	uint32_t x, uint32_t y,
	uint32_t w, uint32_t h
) {
	if (NewColor == OldColor) return false;
	if (floodFillLocStack == NULL) {
		floodFillLocStack = s_init(w * h);
	} else if (floodFillLocStack->length != w * h) {
		s_free(floodFillLocStack);
		floodFillLocStack = s_init(w * h);
	}

	bool didChange = false;
	s_push(floodFillLocStack, x, y);
	while (!s_isEmpty(floodFillLocStack)) {
		int32_t x, y;
		s_pop(floodFillLocStack, &x, &y);

		if (x > -1 && x < w && y > -1 && y < h) {
			Pixel& pixel = Pixels[(y * w) + x];
			if (pixel == OldColor) {
				pixel = NewColor;
				didChange = true;

				int32_t newX = x + 1;
				if (newX < w && Pixels[(y * w) + newX] == OldColor)
					s_push(floodFillLocStack, newX, y);

				newX = x - 1;
				if (newX >= 0 && Pixels[(y * w) + newX] == OldColor)
					s_push(floodFillLocStack, newX, y);

				int32_t newY = y + 1;
				if (newY < h && Pixels[(newY * w) + x] == OldColor)
					s_push(floodFillLocStack, x, newY);

				newY = y - 1;
				if (newY >= 0 && Pixels[(newY * w) + x] == OldColor)
					s_push(floodFillLocStack, x, newY);
			}
		}
	}
	s_clear(floodFillLocStack);

	return didChange;
}

