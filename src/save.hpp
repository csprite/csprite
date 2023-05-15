#ifndef SAVE_H
#define SAVE_H

#include "main.h"
#include "stb/stb_image_write.h"
#include "stb/stb_image.h"
#include "types.hpp"
#include "pixel/pixel.hpp"

// Loads a image to canvas and automatically calls FreeHistory to reset undo/redo
void LoadImageToCanvas(const char *filepath, int *canvas_dims, Pixel** canvas_data) {
	int imgWidth, imgHeight, c;
	unsigned char *image_data = stbi_load(filepath, &imgWidth, &imgHeight, &c, 0);
	if (image_data == NULL) {
		printf("Unable to load image %s\n", filepath);
		return;
	}

	canvas_dims[0] = imgWidth;
	canvas_dims[1] = imgHeight;

	if (*canvas_data != NULL) free(*canvas_data);

	*canvas_data = new Pixel[canvas_dims[0] * canvas_dims[1]]{ 0, 0, 0, 0 };

	unsigned char *ptr;
	unsigned char *iptr;
	for (u32 y = 0; y < imgHeight; y++) {
		for (u32 x = 0; x < imgWidth; x++) {
			Pixel& destPixel = *canvas_data[(y * canvas_dims[0]) + x];
			u8* srcPixel = image_data + (y * canvas_dims[0]) + x;
			destPixel.r = *(srcPixel + 0);
			destPixel.g = *(srcPixel + 1);
			destPixel.b = *(srcPixel + 2);
			destPixel.a = *(srcPixel + 3);
		}
	}
	stbi_image_free(image_data);
	FreeHistory();
}

void WritePngFromCanvas(const char *filepath, int *canvas_dims) {
	unsigned char *data = (unsigned char *) malloc(canvas_dims[0] * canvas_dims[1] * 4 * sizeof(unsigned char));
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_write_png(filepath, canvas_dims[0], canvas_dims[1], 4, data, 0);
	free(data);
}

void WriteJpgFromCanvas(const char *filepath, int *canvas_dims) {
	unsigned char *data = (unsigned char *) malloc(canvas_dims[0] * canvas_dims[1] * 4 * sizeof(unsigned char));
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_write_jpg(filepath, canvas_dims[0], canvas_dims[1], 4, data, 100);
	free(data);
}

#endif // end SAVE_H
