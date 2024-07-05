#include <stdlib.h>
#include <string.h>

#include "image/image.h"
#include "log/log.h"
#include "stb_image.h"

int Image_Create(image_t* out_image, uint32_t width, uint32_t height) {
	if (width < 1 || height < 1) {
		log_error("Got invalid resolution of %dx%d", width, height);
		return 1;
	}

	image_t image;
	image.width = width;
	image.height = height;
	image.pixels = malloc(sizeof(*image.pixels) * width * height);

	if (image.pixels == NULL) {
		log_error("malloc() returned NULL");
		return 1;
	}

	*out_image = image;

	return 0;
}

int Image_LoadFrom(image_t* image, const char* filePath) {
	int width = 0, height = 0, channels = 0;
	uint8_t* pixels = stbi_load(filePath, &width, &height, &channels, 4);

	if (width < 1 || height < 1 || channels != 4 || pixels == NULL) {
		log_error("Failed to load '%s', got w: %d, h: %d, c: %d", filePath, width, height, channels);
		return 1;
	}

	if (Image_Create(image, width, height)) {
		return 1;
	}

	for (uint32_t i = 0; i < (uint32_t)(width * height); i++) {
		image->pixels[i].r = pixels[(i * 4) + 0];
		image->pixels[i].g = pixels[(i * 4) + 1];
		image->pixels[i].b = pixels[(i * 4) + 2];
		image->pixels[i].a = pixels[(i * 4) + 3];
	}

	stbi_image_free(pixels);

	return 0;
}

void Image_Destroy(image_t* image) {
	free(image->pixels);
	memset(image, 0, sizeof(*image));
}
