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

#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define ArrayCount(a) (sizeof((a)) / sizeof((a)[0]))

#endif // SRC_BASE_TYPES_H_INCLUDED_
