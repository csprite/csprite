#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Included For Uint32 Type
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PaletteNameSize 512

typedef Uint32 palette_entry_t;

typedef struct {
	char             name[PaletteNameSize];
	unsigned int     numOfEntries;
	palette_entry_t* entries;
} palette_t;

typedef struct {
	unsigned int  numOfEntries;
	palette_t**    entries;
} palette_arr_t;

int FreePalette(palette_t* palette);
int FreePaletteArr(palette_arr_t* pArr);
palette_t* LoadCsvPalette(const char* csvText);
palette_arr_t* PalletteLoadAll();

#ifdef __cplusplus
}
#endif
