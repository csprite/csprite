#ifndef CSP_DOC_BITMAP_HPP_INCLUDED_
#define CSP_DOC_BITMAP_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"

typedef struct Pixel {
	u8 r, g, b, a;

	Pixel& operator = (Pixel& rhs);
	bool operator == (const Pixel& rhs);
	bool operator != (const Pixel& rhs);
} Pixel;

typedef struct {
	Pixel* pixels;
	u16 width;
	u16 height;
} Bitmap;

#endif // CSP_DOC_BITMAP_HPP_INCLUDED_
