#include "types.hpp"

#define GEN_STRUCT_OVERLOAD_FUNCS(name) \
	name& name::operator = (const name& rhs) { \
		x = rhs.x; \
		y = rhs.y; \
		w = rhs.w; \
		h = rhs.h; \
		return *this; \
	} \
	bool name::operator == (const name& rhs) const { \
		return (x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h); \
	} \
	bool name::operator != (const name& rhs) const { \
		return !(*this != rhs); \
	}

GEN_STRUCT_OVERLOAD_FUNCS(RectI32)
GEN_STRUCT_OVERLOAD_FUNCS(RectF32)

