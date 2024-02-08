#include "types.hpp"
#include "tools/tools.hpp"

RectI32 Tool::Draw(u16 x, u16 y, u16 w, u16 h, bool isRounded, i32 strokeSize, const Pixel &strokeColor, Pixel *pixels) {
	// top left is set to max value to trigger resetting it below
	RectI32 dirty = { w, h, 0, 0 };

	for (i32 dirY = -strokeSize / 2; dirY < strokeSize / 2 + 1; dirY++) {
		for (i32 dirX = -strokeSize / 2; dirX < strokeSize / 2 + 1; dirX++) {
			if (
				x + dirX < 0  ||
				x + dirX >= w ||
				y + dirY < 0  ||
				y + dirY > h
			) continue;

			if (isRounded && (dirX * dirX + dirY * dirY) > (strokeSize / 2 * strokeSize / 2))
				continue;

			i32 affectedX = x + dirX, affectedY = y + dirY;
			if (affectedX < w && affectedY < h) {
				Pixel& pixel = pixels[(affectedY * w) + affectedX];
				pixel = strokeColor;

				if (affectedX < dirty.x) dirty.x = affectedX < 0 ? 0 : affectedX;
				if (affectedY < dirty.y) dirty.y = affectedY < 0 ? 0 : affectedY;
				if (affectedX + 1 > dirty.w) dirty.w = affectedX > w ? w : affectedX + 1;
				if (affectedY + 1 > dirty.h) dirty.h = affectedY > h ? h : affectedY + 1;
			}
		}
	}

	return dirty;
}

// void Tool::DrawBetween(u16 sx, u16 sy, u16 ex, u16 ey, u16 w, u16 h, Pixel& strokeColor, Pixel* pixels) {
// 	while (sx != ex || sy != ey) {
// 		if (sx < ex) sx++;
// 		if (sx > ex) sx--;
// 		if (sy < ey) sy++;
// 		if (sy > ey) sy--;

// 		if (sx < w && sy < h && ex < w && ey < h)
// 			Tool::Draw(sx, sy, w, h, strokeColor, pixels);
// 	}
// }
