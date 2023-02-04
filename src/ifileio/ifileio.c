#include "ifileio.h"
#include "../logger.h"
#include "../utils.h"

#include "stb_image.h"
#include "stb_image_write.h"

int ifio_write(const char* filePath, uint8_t* pixels, int32_t w, int32_t h) {
	if (filePath == NULL || pixels == NULL || w < 1 || h < 1) return -1;

	if (HAS_SUFFIX_CI(filePath, ".png", 4)) {
		stbi_write_png(filePath, w, h, 4, pixels, 0);
	} else if (HAS_SUFFIX_CI(filePath, ".jpeg", 5) || HAS_SUFFIX_CI(filePath, ".jpg", 4)) {
		stbi_write_jpg(filePath, w, h, 4, pixels, 100);
	} else {
		Logger_Error("Error Un-supported file format: %s\n", filePath);
		return -1;
	}

	return 0;
}

uint8_t* ifio_read(const char* filePath, int32_t* w_ptr, int32_t* h_ptr) {
	if (filePath == NULL || w_ptr == NULL || h_ptr == NULL) return NULL;

	if (HAS_SUFFIX_CI(filePath, ".png", 4) || HAS_SUFFIX_CI(filePath, ".jpeg", 5) || HAS_SUFFIX_CI(filePath, ".jpg", 4)) {
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
