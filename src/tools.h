#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void Tools_SetBrushSize(int32_t BrushSize);
int32_t Tools_GetBrushSize();

// Returns true if it did modify the Pixels
bool Tool_Brush(
	unsigned char* Pixels,  // Pixels To Modify
	unsigned char* Color,   // Color To Use
	bool IsCircle,          // Whether To Draw Circle Or Square
	uint32_t x, uint32_t y, // X, Y Position Of Where To Start
	uint32_t w, uint32_t h  // Width Of The Pixel Array
);

// Returns true if it did modify the Pixels
bool Tool_FloodFill(
	unsigned char* Pixels,   // Pixels To Modify
	unsigned char* OldColor, // Old Color
	unsigned char* NewColor, // New Color
	uint32_t x, uint32_t y,  // X, Y Position From Where To Start
	uint32_t w, uint32_t h   // Width & Height Of The Pixel Array
);

#ifdef __cplusplus
}
#endif
