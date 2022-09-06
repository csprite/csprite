#ifndef SAVE_H
#define SAVE_H

#include "macros.h"
#include "log/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// Loads a image to canvas and automatically calls FreeHistory to reset undo/redo
int LoadImageToCanvas(const char* filepath, int (*cvDim)[2], Uint32** canvas_data) {
	int imgWidth, imgHeight, channels;
	unsigned char* image_data = stbi_load(filepath, &imgWidth, &imgHeight, &channels, 0);
	if (image_data == NULL) {
		log_error("unable to load image %s", filepath);
		return -1;
	}

	(*cvDim)[0] = imgWidth;
	(*cvDim)[1] = imgHeight;

	*canvas_data = (Uint32*)malloc(imgWidth * imgHeight * 4 * sizeof(Uint32));
	memset(*canvas_data, 0, imgWidth * imgHeight * 4 * sizeof(Uint32));

	int y, x;
	Uint32* ptr;
	unsigned char* iptr;
	for (y = 0; y < imgHeight; y++) {
		for (x = 0; x < imgWidth; x++) {
			ptr = GetPixel(x, y, NULL);
			if (ptr == NULL) continue;

			if (channels == 3) {
				iptr = image_data + ((y * imgWidth + x) * 3); // Gets Pixel At x, y in image_data
				*ptr = RGBA2UINT32(*(iptr+0), *(iptr+1), *(iptr+2), 255);
			} else if (channels == 4) {
				iptr = image_data + ((y * imgWidth + x) * 4); // Gets Pixel At x, y in image_data
				*ptr = RGBA2UINT32(*(iptr+0), *(iptr+1), *(iptr+2), *(iptr+3));
			}
		}
	}
	stbi_image_free(image_data);
	return 0;
}

void WritePngFromCanvas(const char *filepath, int *canvas_dims, Uint32* data) {
	/*
		Basically stbi takes a pixel array of values rgba like: [r, g, b, a, r, g, b, a...],
		and what we are giving it is: [rgba, rgba, rgba...] so we need to convert it to above format
		and then write it.
	*/

	unsigned char* pixels = (unsigned char*)malloc(canvas_dims[0] * canvas_dims[1] * 4 * sizeof(unsigned char));
	memset(pixels, 0, canvas_dims[0] * canvas_dims[1] * 4 * sizeof(unsigned char));

	unsigned char* ptr;
	for (int x = 0; x < canvas_dims[0]; x++) {
		for (int y = 0; y < canvas_dims[1]; y++) {
			Uint32* pixel = GetPixel(x, y, data);
			ptr = pixels + ((y * canvas_dims[0] + x) * 4);
			*(ptr+0) = (*pixel >> 24 ) & 0xff;
			*(ptr+1) = (*pixel >> 16) & 0xff;
			*(ptr+2) = (*pixel >> 8) & 0xff;
			*(ptr+3) = (*pixel) & 0xff;
		}
	}

	stbi_write_png(filepath, canvas_dims[0], canvas_dims[1], 4, pixels, 0);
	free(pixels);
}

void WriteJpgFromCanvas(const char *filepath, int *canvas_dims, Uint32* data) {
	int channels = 3;
	unsigned char* pixels = (unsigned char*)malloc(canvas_dims[0] * canvas_dims[1] * channels * sizeof(unsigned char));
	memset(pixels, 0, canvas_dims[0] * canvas_dims[1] * channels * sizeof(unsigned char));

	unsigned char* ptr;
	for (int y = 0; y < canvas_dims[1]; y++) {
		for (int x = 0; x < canvas_dims[0]; x++) {
			Uint32* pixel = GetPixel(x, y, data);
			ptr = pixels + ((y * canvas_dims[0] + x) * channels);
			*(ptr+0) = (*pixel & 0xFF000000) >> 24;  // R
			*(ptr+1) = (*pixel & 0x00FF0000) >> 16;  // G
			*(ptr+2) = (*pixel & 0x0000FF00) >> 8;   // B
			// Uncomment If you want to use 4 channels even tho jpeg uses 3.
			// *(ptr+3) = (*pixel & 0x000000FF);        // A
		}
	}

	stbi_write_jpg(filepath, canvas_dims[0], canvas_dims[1], channels, pixels, 100);
	free(pixels);
}

#endif // end SAVE_H
