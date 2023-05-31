#include "tools/tools.hpp"

static u16 _brushSize = 1;
static ToolType _tType = ToolType::BRUSH;
static ToolShape _tShape = ToolShape::CIRCLE;

u16 ToolManager::GetBrushSize() {
	return _brushSize;
}

void ToolManager::SetBrushSize(u16 _sz) {
	if (_sz > 0) _brushSize = _sz;
}

ToolType ToolManager::GetToolType() {
	return _tType;
}

void ToolManager::SetToolType(ToolType _t) {
	_tType = _t;
}

ToolShape ToolManager::GetToolShape() {
	return _tShape;
}

void ToolManager::SetToolShape(ToolShape _s) {
	_tShape = _s;
}

void ToolManager::Draw(u16 x, u16 y, u16 w, u16 h, Pixel& strokeColor, Pixel* pixels) {
	for (i32 dirY = -_brushSize / 2; dirY < _brushSize / 2 + 1; dirY++) {
		for (i32 dirX = -_brushSize / 2; dirX < _brushSize / 2 + 1; dirX++) {
			if (
				x + dirX < 0  ||
				x + dirX >= w ||
				y + dirY < 0  ||
				y + dirY > h
			) continue;

			if (
				_tShape == ToolShape::CIRCLE &&
				(dirX * dirX + dirY * dirY) > (_brushSize / 2 * _brushSize / 2)
			) continue;

			u16 affectedX = x + dirX, affectedY = y + dirY;
			if (affectedX >= 0 && affectedY >= 0 && affectedX < w && affectedY < h) {
				Pixel& pixel = pixels[(affectedY * w) + affectedX];
				pixel = strokeColor;
			}
		}
	}
}

void ToolManager::DrawBetween(u16 sx, u16 sy, u16 ex, u16 ey, u16 w, u16 h, Pixel& strokeColor, Pixel* pixels) {
	while (sx != ex || sy != ey) {
		if (sx < ex) sx++;
		if (sx > ex) sx--;
		if (sy < ey) sy++;
		if (sy > ey) sy--;

		if (
			sx >= 0 && sy >= 0 && ex >= 0 && ey >= 0 &&
			sx < w && sy < h && ex < w && ey < h
		) ToolManager::Draw(sx, sy, w, h, strokeColor, pixels);
	}
}
