#ifndef CSP_TOOLS_TOOLS_HPP_INCLUDED_
#define CSP_TOOLS_TOOLS_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "pixel/pixel.hpp"

namespace Tool {
	// Returns Dirty Area with Top Left & Bottom Right Coords
	RectI32 Draw(
		u16 x, u16 y, u16 w, u16 h,
		bool isRounded, i32 strokeSize,
		const Pixel& strokeColor, Pixel* pixels
	);
	// void DrawBetween(
	// 	u16 startX, u16 startY, u16 endX, u16 endY,
	// 	u16 w, u16 h, const Pixel& strokeColor, Pixel* pixels
	// );
};

#endif // CSP_TOOLS_TOOLS_HPP_INCLUDED_
