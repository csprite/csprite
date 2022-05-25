#ifndef SAVE_H
#define SAVE_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb/stb_image.h"

// Declaration here, implementation in main.cpp
void ResetHistory();

// Loads a image to canvas and automatically calls ResetHistory to reset undo/redo
void LoadImageToCanvas(const char *filepath, int *canvas_dims, unsigned char **canvas_data) {
	int imgWidth, imgHeight, c;
	unsigned char *image_data = stbi_load(filepath, &imgWidth, &imgHeight, &c, 0);
	if (image_data == NULL) {
		printf("Unable to load image %s\n", filepath);
		return;
	}

	canvas_dims[0] = imgWidth;
	canvas_dims[1] = imgHeight;

	if (*canvas_data != NULL) free(*canvas_data);

	*canvas_data = (unsigned char *)malloc(canvas_dims[0] * canvas_dims[1] * 4 * sizeof(unsigned char));
	memset(*canvas_data, 0, canvas_dims[0] * canvas_dims[1] * 4 * sizeof(unsigned char));
	int j, k;
	unsigned char *ptr;
	unsigned char *iptr;
	for (j = 0; j < imgHeight; j++) {
		for (k = 0; k < imgWidth; k++) {
			ptr = GetPixel(k, j);
			iptr = GetCharData(image_data, k, j);
			*(ptr+0) = *(iptr+0);
			*(ptr+1) = *(iptr+1);
			*(ptr+2) = *(iptr+2);
			*(ptr+3) = *(iptr+3);
		}
	}
	stbi_image_free(image_data);
	ResetHistory();
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
