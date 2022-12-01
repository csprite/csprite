#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uchar_t;

void Tools_SetBrushSize(int32_t BrushSize);
int32_t Tools_GetBrushSize();

// Returns true if it did modify the Pixels
bool Tool_Brush(
	uchar_t* Pixels,        // Pixels To Modify
	uchar_t* Color,         // Color To Use
	bool IsCircle,          // Whether To Draw Circle Or Square
	uint32_t x, uint32_t y, // X, Y Position Of Where To Start
	uint32_t w, uint32_t h  // Width & Height Of Pixel Array
);

// Returns true if it did modify the Pixels
bool Tool_Line(
	uchar_t* Pixels,       // Pixels To Modify
	uchar_t* Color,        // Color To Use
	bool IsCircle,         // Wether To Draw Line Rounded Or Not
	int x0, int y0,        // Line Start x, y
	int x1, int y1,        // Line End x, y
	uint32_t w, uint32_t h // Width & Height Of Pixel Array
);

// Returns true if it did modify the Pixels
bool Tool_Rect(
	uchar_t* Pixels,       // Pixels To Modify
	uchar_t* Color,        // Color To Use
	bool IsCircle,         // Wether To Draw Rounded Rectangle Or Not
	int x0, int y0,        // Rectangle Start x, y
	int x1, int y1,        // Rectangle End x, y
	uint32_t w, uint32_t h // Width & Height Of Pixel Array
);

// Returns true if it did modify the Pixels
bool Tool_Circle(
	uchar_t* Pixels,          // Pixels To Modify
	uchar_t* Color,           // Color To Use
	int centreX, int centreY, // Circle Center x, y
	int radius,               // Circle's Radius
	uint32_t w, uint32_t h    // Width & Height Of Pixel Array
);

// Returns true if it did modify the Pixels
bool Tool_FloodFill(
	uchar_t* Pixels,   // Pixels To Modify
	uchar_t* OldColor, // Old Color
	uchar_t* NewColor, // New Color
	uint32_t x, uint32_t y,  // X, Y Position From Where To Start
	uint32_t w, uint32_t h   // Width & Height Of The Pixel Array
);

#ifdef __cplusplus
}
#endif
