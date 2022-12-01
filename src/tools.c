#include "tools.h"
#include "utils.h"
#include "macros.h"
#include "stb_image.h"

int32_t BrushSize = 1;

int32_t Tools_GetBrushSize() {
	return BrushSize;
}

void Tools_SetBrushSize(int32_t NewBrushSize) {
	BrushSize = NewBrushSize;
}

bool Tool_Brush(unsigned char* Pixels, unsigned char* Color, bool IsCircle, uint32_t st_x, uint32_t st_y, uint32_t w, uint32_t h) {
	bool didChange = false;
	// dirY = direction Y
	// dirX = direction X

	// Loops From -BrushSize/2 To BrushSize/2, ex: -6/2 to 6/2 -> -3 to 3
	for (int dirY = -BrushSize / 2; dirY < BrushSize / 2 + 1; dirY++) {
		for (int dirX = -BrushSize / 2; dirX < BrushSize / 2 + 1; dirX++) {
			if (st_x + dirX < 0 || st_x + dirX >= w || st_y + dirY < 0 || st_y + dirY > h)
				continue;

			if (IsCircle == true && dirX * dirX + dirY * dirY > BrushSize / 2 * BrushSize / 2)
				continue;

			unsigned char* pixel = GetCharData(Pixels, st_x + dirX, st_y + dirY, w, h);
			if (pixel != NULL) {
				*(pixel + 0) = Color[0];
				*(pixel + 1) = Color[1];
				*(pixel + 2) = Color[2];
				*(pixel + 3) = Color[3];
				didChange = true;
			}
		}
	}
	return didChange;
}

bool Tool_FloodFill(
	unsigned char* Pixels,
	unsigned char* OldColor,
	unsigned char* NewColor,
	uint32_t x, uint32_t y,
	uint32_t w, uint32_t h
) {
	bool didChange = false;
	if (
		x >= 0 && y >= 0 && x < w && y < h &&
		Pixels != NULL && OldColor != NULL && NewColor != NULL
	) {
		unsigned char* pixel = GetCharData(Pixels, x, y, w, h);

		if (pixel != NULL) {
			if (COLOR_EQUAL(pixel, OldColor) == 1) {
				*pixel = NewColor[0];
				*(pixel + 1) = NewColor[1];
				*(pixel + 2) = NewColor[2];
				*(pixel + 3) = NewColor[3];

				if (x + 1 < w)  didChange = didChange || Tool_FloodFill(Pixels, OldColor, NewColor, x + 1, y, w, h);
				if (x - 1 >= 0) didChange = didChange || Tool_FloodFill(Pixels, OldColor, NewColor, x - 1, y, w, h);
				if (y + 1 < h)  didChange = didChange || Tool_FloodFill(Pixels, OldColor, NewColor, x, y + 1, w, h);
				if (x + 1 >= 0) didChange = didChange || Tool_FloodFill(Pixels, OldColor, NewColor, x, y - 1, w, h);
			}
		}
	}
	return didChange;
}
