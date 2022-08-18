#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PaletteNameSize 512

typedef uint8_t palette_entry_t[4];

typedef struct {
	char             name[PaletteNameSize];
	unsigned int     numOfEntries;
	palette_entry_t *entries;
} palette_t;

palette_t* LoadCsvPalette(const char* csvText);
int FreePalette(palette_t* palette);

#ifdef __cplusplus
}
#endif
