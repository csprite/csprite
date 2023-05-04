#ifndef CSP_DOC_BITMAP_HPP_INCLUDED_
#define CSP_DOC_BITMAP_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"

struct Pixel {
	u8 r, g, b, a;

	Pixel& operator = (Pixel& rhs);
	inline bool operator == (const Pixel& rhs) const;
	inline bool operator != (const Pixel& rhs) const;
};

enum BlendMode {
	NORMAL = 0
};

struct BitmapLayer {
	String name;
	Pixel* pixels;
	BlendMode bMode;
};

struct Bitmap {
	u16 width;
	u16 height;
	Pixel* render;
	Vector<BitmapLayer> layers;
};

#endif // CSP_DOC_BITMAP_HPP_INCLUDED_
