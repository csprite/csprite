#include <stdlib.h>
#include <string.h>
#include "palette/palette.h"
#include "log/log.h"

int Palette_Create(palette_t* out_pal, uint32_t count, pixel_t* colors) {
	if (count < 1 || colors == NULL) {
		log_error("Got invalid input, count: %d, colors: %p", count, colors);
		return 1;
	}

	palette_t pal;
	pal.count = count;
	pal.colors = malloc(sizeof(*pal.colors) * count);

	if (pal.colors == NULL) {
		log_error("malloc() returned NULL");
		return 1;
	}

	for (uint32_t i = 0; i < count; i++) {
		pal.colors[i] = colors[i];
	}

	*out_pal = pal;

	return 0;
}

int Palette_Push(palette_t* pal, pixel_t color) {
	pal->colors = realloc(pal->colors, sizeof(*pal->colors) * ++pal->count);
	return 0;
}

void Palette_Destroy(palette_t* pal) {
	free(pal->colors);
	memset(pal, 0, sizeof(*pal));
}
