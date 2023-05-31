#ifndef CSP_TOOLS_TOOLS_HPP_INCLUDED_
#define CSP_TOOLS_TOOLS_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "pixel/pixel.hpp"

enum ToolType : u8 {
	BRUSH = 0,
	ERASER,
	PAN,
	INK_DROPPER
};

enum ToolShape : u8 {
	SQUARE = 0,
	CIRCLE
};

namespace ToolManager {
	u16 GetBrushSize();
	void SetBrushSize(u16 sz);

	ToolType GetToolType();
	void SetToolType(ToolType _t);

	ToolShape GetToolShape();
	void SetToolShape(ToolShape _s);

	void Draw(u16 x, u16 y, u16 w, u16 h, Pixel& strokeColor, Pixel* pixels);
	void DrawBetween(
		u16 startX, u16 startY, u16 endX, u16 endY,
		u16 w, u16 h, Pixel& strokeColor, Pixel* pixels
	);
};

#endif // CSP_TOOLS_TOOLS_HPP_INCLUDED_
