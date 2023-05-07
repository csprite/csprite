#include "types.hpp"

inline Rect& Rect::operator = (const Rect& rhs) {
	x = rhs.x;
	y = rhs.y;
	w = rhs.w;
	h = rhs.h;
	return *this;
}

inline bool Rect::operator == (const Rect& rhs) const {
	return (x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h);
}

inline bool Rect::operator != (const Rect& rhs) const {
	return !(*this != rhs);
}

inline Rect::operator SDL_Rect() const {
	return { x, y, w, h };
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
