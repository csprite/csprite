#include "tools/tools.hpp"

void Tool::Draw(u16 x, u16 y, u16 w, u16 h, bool isRounded, i32 strokeSize, const Pixel &strokeColor, Pixel *pixels) {
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

			u16 affectedX = x + dirX, affectedY = y + dirY;
			if (affectedX < w && affectedY < h) {
				Pixel& pixel = pixels[(affectedY * w) + affectedX];
				pixel = strokeColor;
			}
		}
	}
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
