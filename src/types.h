#ifndef CSPRITE_SRC_TYPES_H_INCLUDED_
#define CSPRITE_SRC_TYPES_H_INCLUDED_
#pragma once

#include <stdint.h>

typedef struct {
	int32_t x, y;
} Vec2_t;

typedef struct {
	Vec2_t start, end;
} Rect_t;

#endif
