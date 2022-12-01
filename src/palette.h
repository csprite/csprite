#ifndef PALETTE_H
#define PALETTE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PaletteNameSize 512
#define PaletteAuthorSize 512 + 17 // The Extra 19 Are For "Awesome Palette By ", so the actual name starts after 19 Characters

typedef unsigned char palette_entry_t[4];

typedef struct {
	char             name[PaletteNameSize];
	char             author[PaletteAuthorSize];
	unsigned int     numOfEntries;
	palette_entry_t*  Colors;
} Palette_T;

typedef struct {
	unsigned int   numOfEntries;
	Palette_T**    Palettes;
} PaletteArr_T;

int FreePalette(Palette_T* palette);
int FreePaletteArr(PaletteArr_T* pArr);
Palette_T* LoadCsvPalette(const char* csvText);
PaletteArr_T* PaletteLoadAll();

#ifdef __cplusplus
}
#endif

#endif // PALETTE_H
