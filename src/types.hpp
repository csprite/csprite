#ifndef CSP_TYPES_HPP_INCLUDED_
#define CSP_TYPES_HPP_INCLUDED_ 1
#pragma once

// Functions & Definitions To Work With Different Types, Like Short Aliases, Comparisons, Clamping

#include <cstdint>
#include <string>
#include <vector>
#include <limits>

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
		name& operator = (const name& rhs); \
		bool operator == (const name& rhs) const; \
		bool operator != (const name& rhs) const; \
	}

GEN_RECT_STRUCT(RectI32, i32);
GEN_RECT_STRUCT(RectF32, f32);

// Clamps "a" to the min & max value
#define CLAMP_NUM(a, min, max) \
	((a <= min ? min : a ) >= max ? max : (a <= min ? min : a))

// Clamps "a" to the min & max value of "targetType", without overflowing.
#define CLAMP_NUM_TO_TYPE(a, targetType)             \
	static_cast<targetType>(                         \
		CLAMP_NUM(                                   \
			a,                                       \
			std::numeric_limits<targetType>().min(), \
			std::numeric_limits<targetType>().max()  \
		)                                            \
	)

#endif // CSP_TYPES_HPP_INCLUDED_
