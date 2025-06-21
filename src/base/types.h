#ifndef SRC_BASE_TYPES_H_INCLUDED_
#define SRC_BASE_TYPES_H_INCLUDED_ 1
#pragma once

#include <stdint.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

typedef S8 B8;
typedef S16 B16;
typedef S32 B32;

typedef float F32;
typedef double F64;

typedef struct {
	U8 r, g, b, a;
} Pixel;

typedef struct {
	F32 r, g, b;
} RGBF32;

typedef struct {
	F32 r, g, b, a;
} RGBAF32;

#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define ArrayCount(a) (sizeof((a)) / sizeof((a)[0]))
#define AlignUpPow2(x,b) (((x) + (b) - 1)&(~((b) - 1)))

// Branch Prediction Hints
#if defined(__clang__) || defined(__GNUC__)
	#define Expect(expr, val) __builtin_expect((expr), (val))
#else
	#define Expect(expr, val) (expr)
#endif

#define Likely(expr)   Expect(expr, 1)
#define Unlikely(expr) Expect(expr, 0)

// Units
#define KB(n) (((U64)(n)) << 10)
#define MB(n) (((U64)(n)) << 20)
#define GB(n) (((U64)(n)) << 30)

// Memory Operations
#define MemoryZero(s,z)     memset((s),0,(z))
#define MemoryZeroStruct(s) MemoryZero((s),sizeof(*(s)))

// For-Loop Constructs
#define EachIndex(it, count) (U64 it = 0; it < (count); it += 1)
#define EachElement(it, array) (U64 it = 0; it < ArrayCount(array); it += 1)
#define EachEnumVal(type, it) (type it = (type)0; it < type##_COUNT; it = (type)(it+1))

#endif // SRC_BASE_TYPES_H_INCLUDED_
