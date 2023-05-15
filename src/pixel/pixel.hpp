#ifndef CSP_SRC_PIXEL_PIXEL_HPP_INCLUDED_
#define CSP_SRC_PIXEL_PIXEL_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"

#ifndef TARGET_IS_BIGENDIAN
	#define PIXEL_RED_MASK   0xFF000000
	#define PIXEL_GREEN_MASK 0x00FF0000
	#define PIXEL_BLUE_MASK  0x0000FF00
	#define PIXEL_ALPHA_MASK 0x000000FF

	#define PIXEL_RED_SHIFT   24
	#define PIXEL_GREEN_SHIFT 16
	#define PIXEL_BLUE_SHIFT  8
	#define PIXEL_ALPHA_SHIFT 0
#else
	#define PIXEL_RED_MASK   0x000000FF
	#define PIXEL_GREEN_MASK 0x0000FF00
	#define PIXEL_BLUE_MASK  0x00FF0000
	#define PIXEL_ALPHA_MASK 0xFF000000

	#define PIXEL_RED_SHIFT   0
	#define PIXEL_GREEN_SHIFT 8
	#define PIXEL_BLUE_SHIFT  16
	#define PIXEL_ALPHA_SHIFT 24
#endif

struct Pixel {
	u8 r = 0, g = 0, b = 0, a = 0;

	Pixel& operator = (const u32 RGBA_Color);
	Pixel& operator = (const Pixel& rhs);
	bool operator == (const Pixel& rhs) const;
	bool operator != (const Pixel& rhs) const;
	explicit operator u8*(); // returns the pointer to the first element in the struct and other elements can be acced via struct[index]
};

#endif // CSP_SRC_PIXEL_PIXEL_HPP_INCLUDED_
