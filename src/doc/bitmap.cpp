#include "doc/bitmap.hpp"

Pixel& Pixel::operator = (Pixel& rhs) {
	r = rhs.r;
	g = rhs.g;
	b = rhs.b;
	a = rhs.a;
	return *this;
}

bool Pixel::operator == (const Pixel& rhs) {
	return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
}

bool Pixel::operator != (const Pixel& rhs) {
	return !(*this == rhs);
}

