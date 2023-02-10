#include "ifileio.h"
#include "../utils.h"
#include "../logger.h"
#include "../renderer/renderer.h"

#include "stb_image.h"
#include "stb_image_write.h"

static uint8_t clamp_u16_to_u8(uint16_t val) {
	if (val > -1 && val < 256) return val;
	else if (val > 255) return 255;

	return 0;
}

static uint8_t* BlendPixels_Alpha(int32_t w, int32_t h, CanvasLayerArr_T* arr) {
	uint8_t* canvas_data = (uint8_t*) malloc(w * h * 4 * sizeof(uint8_t));
	memset(canvas_data, 0, w * h * 4 * sizeof(uint8_t));

	for (uint32_t i = 0; i < arr->size; ++i) {
		if (arr->layers[i] != NULL) {
			for (int32_t y = 0; y < h; ++y) {
				for (int32_t x = 0; x < w; ++x) {
					// Simple Alpha-Blending Being Done Here.
					uint8_t* srcPixel = GetCharData(arr->layers[i]->pixels, x, y, w, h);
					uint8_t* destPixel = GetCharData(canvas_data, x, y, w, h);
					if (srcPixel != NULL && destPixel != NULL) {
						uint8_t src1Red = *(srcPixel + 0), src1Green = *(srcPixel + 1), src1Blue = *(srcPixel + 2), src1Alpha = *(srcPixel + 3);
						uint8_t src2Red = *(destPixel + 0), src2Green = *(destPixel + 1), src2Blue = *(destPixel + 2), src2Alpha = *(destPixel + 3);

						uint16_t outRed = ((uint16_t)src1Red * src1Alpha + (uint16_t)src2Red * (255 - src1Alpha) / 255 * src2Alpha) / 255;
						uint16_t outGreen = ((uint16_t)src1Green * src1Alpha + (uint16_t)src2Green * (255 - src1Alpha) / 255 * src2Alpha) / 255;
						uint16_t outBlue = ((uint16_t)src1Blue * src1Alpha + (uint16_t)src2Blue * (255 - src1Alpha) / 255 * src2Alpha) / 255;
						uint16_t outAlpha = src1Alpha + (uint16_t)src2Alpha * (255 - src1Alpha) / 255;

						*(destPixel + 0) = clamp_u16_to_u8(outRed);
						*(destPixel + 1) = clamp_u16_to_u8(outGreen);
						*(destPixel + 2) = clamp_u16_to_u8(outBlue);
						*(destPixel + 3) = clamp_u16_to_u8(outAlpha);
					}
				}
			}
		}
	}
	return canvas_data;
}

int32_t ifio_write(const char* filePath, int32_t w, int32_t h, CanvasLayerArr_T* arr) {
	if (filePath == NULL || arr == NULL || w < 1 || h < 1) return -1;

	uint8_t* _BlendedPixels = NULL;
	if (HAS_SUFFIX_CI(filePath, ".png", 4)) {
		_BlendedPixels = BlendPixels_Alpha(w, h, arr);
		if (_BlendedPixels == NULL) {
			Logger_Error("Alpha Blending Failed, BlendPixels_Alpha(...) returned NULL");
			return -1;
		} else {
			stbi_write_png(filePath, w, h, 4, _BlendedPixels, 0);
			free(_BlendedPixels);
			_BlendedPixels = NULL;
		}
	} else if (HAS_SUFFIX_CI(filePath, ".jpeg", 5) || HAS_SUFFIX_CI(filePath, ".jpg", 4)) {
		_BlendedPixels = BlendPixels_Alpha(w, h, arr);
		if (_BlendedPixels == NULL) {
			Logger_Error("Alpha Blending Failed, BlendPixels_Alpha(...) returned NULL");
			return -1;
		} else {
			stbi_write_jpg(filePath, w, h, 4, _BlendedPixels, 100);
			free(_BlendedPixels);
			_BlendedPixels = NULL;
		}
	} else {
		Logger_Error("Error Un-supported file format: %s\n", filePath);
		return -1;
	}

	return 0;
}

int32_t ifio_read(const char* filePath, int32_t* w_ptr, int32_t* h_ptr, CanvasLayerArr_T** arr) {
	if (filePath == NULL || arr == NULL || *arr == NULL || w_ptr == NULL || h_ptr == NULL) return -1;

	if (HAS_SUFFIX_CI(filePath, ".png", 4) || HAS_SUFFIX_CI(filePath, ".jpeg", 5) || HAS_SUFFIX_CI(filePath, ".jpg", 4)) {
		int32_t w = 0, h = 0, channels = 0;
		uint8_t* _data = stbi_load(filePath, &w, &h, &channels, 4);
		if (w > 0 && h > 0 && _data) {
			*w_ptr = (uint32_t)w;
			*h_ptr = (uint32_t)h;
			Canvas_DestroyArr(*arr);
			Canvas_Resize(w, h, R_GetRenderer());
			CanvasLayer_T* layer = Canvas_CreateLayer(R_GetRenderer());
			*arr = Canvas_CreateArr(100);
			(*arr)->size++;
			(*arr)->layers[0] = layer;

			memcpy(layer->pixels, _data, w * h * 4 * sizeof(uint8_t));
			memcpy(layer->history->pixels, _data, w * h * 4 * sizeof(uint8_t));
			SaveHistory(&layer->history, w * h * 4 * sizeof(uint8_t), layer->pixels);
			return 0;
		}
	} else {
		Logger_Error("Error Un-supported file format: %s\n", filePath);
	}
	return -1;
}

