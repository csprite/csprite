#ifndef SRC_PALETTE_PALETTE_H_INCLUDED_
#define SRC_PALETTE_PALETTE_H_INCLUDED_
#pragma once

#include "gfx/gfx.h"

typedef struct {
	pixel_t* colors;
	uint32_t count;
} palette_t;

int Palette_Create(palette_t* pal, uint32_t count, pixel_t* colors);
int Palette_Push(palette_t* pal, pixel_t color);
void Palette_Destroy(palette_t* pal);

#endif
