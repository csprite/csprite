#ifndef CSPRITE_SRC_TYPES_H_INCLUDED_
#define CSPRITE_SRC_TYPES_H_INCLUDED_
#pragma once

#include <stdint.h>

typedef struct {
	int32_t x, y;
} Vec2_t;

typedef struct {
	int64_t min_x, min_y, max_x, max_y;
} mmRect_t;

#endif
