#ifndef CSP_TOOLS_TOOLS_HPP_INCLUDED_
#define CSP_TOOLS_TOOLS_HPP_INCLUDED_ 1
#pragma once

#include "types.h"
#include "pixel/pixel.h"

namespace Tool {
	// Returns Dirty Area with Top Left & Bottom Right Coords
	RectU32 Draw(
		u32 x, u32 y, u32 w, u32 h,
		bool isRounded, i32 strokeSize,
		const Pixel& strokeColor, Pixel* pixels
	);
	RectU32 Line(
		u32 startX, u32 startY, u32 endX, u32 endY,
		u32 w, u32 h, bool isRounded, i32 strokeSize,
		const Pixel& strokeColor, Pixel* pixels
	);
};

#endif // CSP_TOOLS_TOOLS_HPP_INCLUDED_
