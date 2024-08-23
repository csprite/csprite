#ifndef CSPRITE_SRC_PIXEL_H_INCLUDED_
#define CSPRITE_SRC_PIXEL_H_INCLUDED_
#pragma once

#include <stdint.h>

typedef struct {
	uint8_t r, g, b, a;
} pixel_t;

#define PIXEL_EQUAL(p1, p2) (p1.r == p2.r && p1.g == p2.g && p1.b == p2.b && p1.a == p2.a)

#endif
