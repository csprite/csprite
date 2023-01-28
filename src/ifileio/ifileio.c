#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "ifileio.h"
#include "../logger.h"

#include "stb_image.h"
#include "stb_image_write.h"

/* Case insensitive string compare upto n. XX-TODO: The Function also exists in imgui_styles.h, move the function to utils.h */
static int strncmpci(const char* s1, const char* s2, size_t n) {
	while (n && *s1 && ( tolower(*s1) == tolower(*s2) )) {
		++s1;
		++s2;
		--n;
	}
	if (n == 0) return 0;
	else return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}

int ifio_write(const char* filePath, uint8_t* pixels, int32_t w, int32_t h) {
	if (filePath == NULL || pixels == NULL || w < 1 || h < 1) return -1;

	// Checks if a string has a suffix, it is case-insensitive.
	#define HAS_SUFFIX(str, ext, size) strncmpci(str + (strlen(str) - size), ext, size) == 0

	if (HAS_SUFFIX(filePath, ".png", 4)) {
		stbi_flip_vertically_on_write(1); // Flip Vertically Because Of OpenGL's Coordinate System
		stbi_write_png(filePath, w, h, 4, pixels, 0);
	} else if (HAS_SUFFIX(filePath, ".jpeg", 5) || HAS_SUFFIX(filePath, ".jpg", 4)) {
		stbi_flip_vertically_on_write(1); // Flip Vertically Because Of OpenGL's Coordinate System
		stbi_write_jpg(filePath, w, h, 4, pixels, 100);
	} else {
		Logger_Error("Error Un-supported file format: %s\n", filePath);
		return -1;
	}

	return 0;
}

uint8_t* ifio_read(const char* filePath, int32_t* w_ptr, int32_t* h_ptr) {
	if (filePath == NULL || w_ptr == NULL || h_ptr == NULL) return NULL;

	// Checks if a string has a suffix, it is case-insensitive.
	#define HAS_SUFFIX(str, ext, size) strncmpci(str + (strlen(str) - size), ext, size) == 0

	if (HAS_SUFFIX(filePath, ".png", 4) || HAS_SUFFIX(filePath, ".jpeg", 5) || HAS_SUFFIX(filePath, ".jpg", 4)) {
		int w = 0, h = 0, channels = 0;
		uint8_t* _data = stbi_load(filePath, &w, &h, &channels, 4);
		if (w > 0 && h > 0 && _data) {
			*w_ptr = (uint32_t)w;
			*h_ptr = (uint32_t)h;
			return _data;
		}
	} else {
		Logger_Error("Error Un-supported file format: %s\n", filePath);
	}

	return NULL;
}
