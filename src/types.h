#ifndef CSPRITE_SRC_TYPES_H_INCLUDED_
#define CSPRITE_SRC_TYPES_H_INCLUDED_
#pragma once

#include <stdint.h>

typedef struct {
	int32_t x, y;
} Vec2;

typedef struct {
	Vec2 start, end;
} Rect;

static inline void rect_invalidate(Rect* r) {
	r->start.x = r->end.x = 0;
}

static inline int rect_is_valid(Rect* r) {
	return r->start.x != r->end.x;
}

#endif
