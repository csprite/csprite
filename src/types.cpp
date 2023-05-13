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
