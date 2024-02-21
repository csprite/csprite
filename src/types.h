#ifndef CSP_TYPES_H_INCLUDED_
#define CSP_TYPES_H_INCLUDED_ 1
#pragma once

#define MIN(a, min) (a <= min ? min : a)
#define MAX(a, max) (a >= max ? max : a)
#define MIN_MAX(a, min, max) MAX(MIN(a, min), max)

#ifdef __cplusplus
	#include <cstdint>
	#include <string>
	#include <vector>

	typedef std::string String;

	template<typename Type>
	using Vector = std::vector<Type>;
#else
	#include <stdint.h>
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

typedef float f32;
typedef double f64;

typedef char* cString;

#define VEC(type, name) struct name { type x, y; }
#define RECT(type, name) struct name { type x, y, w, h; }
#define RECT_MM(type, name) struct name { type min_x, min_y, max_x, max_y; }

VEC(i32, VecI32);
VEC(f32, VecF32);

RECT(u32, RectU32);
RECT(i32, RectI32);
RECT(f32, RectF32);

RECT_MM(u32, mm_RectU32);

#endif // CSP_TYPES_H_INCLUDED_
