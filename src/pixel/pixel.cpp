#include "pixel/pixel.hpp"

Pixel& Pixel::operator = (Pixel& rhs) {
	r = rhs.r;
	g = rhs.g;
	b = rhs.b;
	a = rhs.a;
	return *this;
}

inline bool Pixel::operator == (const Pixel& rhs) const {
	return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
}

inline bool Pixel::operator != (const Pixel& rhs) const {
	return !(*this == rhs);
}
