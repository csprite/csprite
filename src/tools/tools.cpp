#include "types.hpp"
#include "tools/tools.hpp"

RectU32 Tool::Draw(u32 x, u32 y, u32 w, u32 h, bool isRounded, i32 strokeSize, const Pixel& strokeColor, Pixel* pixels) {
	// top left is set to max value to trigger resetting it below
	RectU32 dirty = { w, h, 0, 0 };

	for (i32 dirY = -strokeSize / 2; dirY < strokeSize / 2 + 1; dirY++) {
		for (i32 dirX = -strokeSize / 2; dirX < strokeSize / 2 + 1; dirX++) {
			i32 affectedX = x + dirX, affectedY = y + dirY;

			if (
				affectedX < 0  ||
				affectedY < 0  ||
				affectedX >= w ||
				affectedY >= h
			) continue;

			if (isRounded && (dirX * dirX + dirY * dirY) > (strokeSize / 2 * strokeSize / 2))
				continue;

			if (affectedX < w && affectedY < h) {
				Pixel& pixel = pixels[(affectedY * w) + affectedX];
				if (pixel != strokeColor) {
					pixel = strokeColor;

					if (affectedX < dirty.x) dirty.x = affectedX < 0 ? 0 : affectedX;
					if (affectedY < dirty.y) dirty.y = affectedY < 0 ? 0 : affectedY;
					if (affectedX + 1 > dirty.w) dirty.w = affectedX > w ? w : affectedX + 1;
					if (affectedY + 1 > dirty.h) dirty.h = affectedY > h ? h : affectedY + 1;
				}
			}
		}
	}

	return dirty;
}

RectU32 Tool::Line(
	u32 sx, u32 sy, u32 ex, u32 ey,
	u32 w, u32 h, bool isRounded, i32 strokeSize,
	const Pixel& strokeColor, Pixel* pixels
) {
	RectU32 dirty = { h, w, 0, 0 };
	while (sx != ex || sy != ey) {
		if (sx < ex) sx++;
		if (sx > ex) sx--;
		if (sy < ey) sy++;
		if (sy > ey) sy--;

		if (sx < w && sy < h && ex < w && ey < h) {
			RectU32 dirtyNew = Tool::Draw(sx, sy, w, h, isRounded, strokeSize, strokeColor, pixels);

			if (dirtyNew.x < dirty.x) dirty.x = dirtyNew.x;
			if (dirtyNew.y < dirty.y) dirty.y = dirtyNew.y;
			if (dirtyNew.x + 1 > dirty.w) dirty.w = dirtyNew.x > w ? w : dirtyNew.x + 1;
			if (dirtyNew.y + 1 > dirty.h) dirty.h = dirtyNew.y > h ? h : dirtyNew.y + 1;
		}
	}
	return dirty;
}
