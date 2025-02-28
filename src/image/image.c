#include <string.h>
#include "base/memory.h"
#include "image/image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "log/log.h"
#include "fs/fs.h"

void image_init(Image* img, uint32_t width, uint32_t height) {
	img->width = width;
	img->height = height;
	img->pixels = Memory_AllocOrDie(width * height * sizeof(Pixel));
	Memory_ZeroAll(img->pixels, width * height * sizeof(Pixel));
}

int image_initFrom(Image* img, const char* filePath) {
	int w = 0, h = 0, c = 0;
	stbi_uc* data = stbi_load(filePath, &w, &h, &c, 4);
	if (data == NULL) {
		log_error("stbi_load(...) returned NULL");
		return 1;
	}
	if (w < 1 || h < 1 || c != 4) {
		log_error("Invalid/Unexpected Image Properties: %dx%dx%d", w, h, c);
		stbi_image_free(data);
		return 1;
	}

	image_init(img, w, h);
	for (long long i = 0; i < w * h; i++) {
		Pixel* out = &img->pixels[i];
		out->r = data[(i * 4) + 0];
		out->g = data[(i * 4) + 1];
		out->b = data[(i * 4) + 2];
		out->a = data[(i * 4) + 3];
	}

	stbi_image_free(data);

	return 0;
}

int image_write(Image* img, const char* filePath) {
	int extensionIdx = fs_get_extension(filePath);
	if (extensionIdx < 0) {
		log_error("Failed to find extension of '%s'", filePath);
		return 1;
	}

	const char* extension = filePath + extensionIdx;
	if (strcmp(extension, ".png") == 0) {
		stbi_write_png_compression_level = 9;
		stbi_write_png(filePath, img->width, img->height, 4, img->pixels, img->width * sizeof(Pixel));
	} else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
		stbi_write_jpg(filePath, img->width, img->height, 4, img->pixels, 100);
	} else {
		log_error("Unsupported extension '%s'", extension);
		return 1;
	}

	return 0;
}

void image_deinit(Image* img) {
	Memory_Dealloc(img->pixels);
	Memory_ZeroAll(img, sizeof(*img));
}
