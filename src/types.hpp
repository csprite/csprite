#ifndef CSP_TYPES_HPP_INCLUDED_
#define CSP_TYPES_HPP_INCLUDED_ 1
#pragma once

// Functions & Definitions To Work With Different Types, Like Short Aliases, Comparisons, Clamping

#include <cstdint>
#include <string>
#include <vector>

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using f32 = float;
using f64 = double;

using String = std::string;

template<typename Type>
using Vector = std::vector<Type>;

#define GEN_RECT_STRUCT(name, type) \
	struct name { \
		type x, y, w, h; \
		inline bool operator == (const name& rhs) const { \
			return (x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h); \
		} \
		inline bool operator != (const name& rhs) const { \
			return !(*this == rhs); \
		} \
	}

GEN_RECT_STRUCT(RectI32, i32);
GEN_RECT_STRUCT(RectU32, u32);
GEN_RECT_STRUCT(RectF32, f32);

#define GEN_VECT_STRUCT(name, type) \
	struct name { type x, y; }

GEN_VECT_STRUCT(VecI32, i32);
GEN_VECT_STRUCT(VecF32, f32);

#define MIN(a, min) (a <= min ? min : a)
#define MAX(a, max) (a >= max ? max : a)
#define MIN_MAX(a, min, max) MAX(MIN(a, min), max)

#endif // CSP_TYPES_HPP_INCLUDED_

