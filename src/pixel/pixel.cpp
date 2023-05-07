#include "pixel/pixel.hpp"

Pixel& Pixel::operator = (const u32 RGBA_Color) {
	r = (RGBA_Color & PIXEL_RED_MASK)   >> PIXEL_RED_SHIFT;
	g = (RGBA_Color & PIXEL_GREEN_MASK) >> PIXEL_GREEN_SHIFT;
	b = (RGBA_Color & PIXEL_BLUE_MASK)  >> PIXEL_BLUE_SHIFT;
	a = (RGBA_Color & PIXEL_ALPHA_MASK) >> PIXEL_ALPHA_SHIFT;
	return *this;
}

Pixel& Pixel::operator = (const Pixel& rhs) {
	r = rhs.r;
	g = rhs.g;
	b = rhs.b;
	a = rhs.a;
	return *this;
}

bool Pixel::operator == (const Pixel& rhs) const {
	return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
}

bool Pixel::operator != (const Pixel& rhs) const {
	return !(*this == rhs);
}

Pixel::operator u8*() {
#if TARGET_IS_BIGENDIAN == 0
	return &r;
#else
	return &a;
#endif
}
