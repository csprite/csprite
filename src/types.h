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

static inline void rect_invalidate(Rect_t* r) {
	r->start.x = r->end.x = 0;
}

static inline int rect_is_valid(Rect_t* r) {
	return r->start.x != r->end.x;
}

#endif
