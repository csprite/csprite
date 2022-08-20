#define _SVID_SOURCE

#include "palette.h"
#include <string.h>
#include "log/log.h"

int FreePalette(palette_t* palette) {
	if (palette == NULL) {
		log_error("Palette Pointer is NULL!");
		return -1;
	}

	if (palette->entries == NULL) {
		log_error("Palette Entries are NULL!");
		return -1;
	}

	free(palette->entries);
	free(palette);
	return 0;
}

palette_t* LoadCsvPalette(const char* csvText) {
	palette_t* newPalette = malloc(sizeof(palette_t));
	if (newPalette == NULL) {
		log_error("'malloc' returned NULL, cannot allocate memory for the palette!");
		return NULL;
	}

	char* text = strdup(csvText);
	char* token = NULL;
	int totalSize = 0;

	// First We Find Out Total Elements, To Allocate Array
	token = strtok(text, ",");
	while(token != NULL) {
		token = strtok(NULL, ",");
		++totalSize;
	}

	free(text);
	text = NULL;
	text = strdup(csvText);

	newPalette->numOfEntries = totalSize - 2; // Number Of Colors = Total Size - 2 (Palette Name, Author)
	newPalette->entries = malloc(sizeof(palette_entry_t) * newPalette->numOfEntries);

	// First We Find Out Total Elements, To Allocate Array
	token = strtok(text, ","); // Palette Name
	strcpy(newPalette->name, token);
	token = strtok(NULL, ","); // Author Name
	token = strtok(NULL, ","); // First Color

	for (int i = 0; token != NULL; ++i) {
		unsigned int r = 0, g = 0, b = 0;
		sscanf(token, "%02x%02x%02x", &r, &g, &b);
		newPalette->entries[i] = RGBA2UINT32(r, g, b, 255);
		token = strtok(NULL, ",");
	}

	free(text);
	text = NULL;

	if (token != NULL) {
		free(token);
		token = NULL;
	}

	return newPalette;
}
