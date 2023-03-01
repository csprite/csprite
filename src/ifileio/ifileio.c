#include <zlib.h> // for uLongf
#include "log/log.h"
#include "ifileio.h"
#include "zlib_wrapper.h"
#include "../utils.h"
#include "../renderer/renderer.h"
#include "../renderer/canvas.h"
#include "ifileio_endian.h"

#include "stb_image.h"
#include "stb_image_write.h"

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

						*(destPixel + 0) = CLAMP_NUM(outRed,   0, 255);
						*(destPixel + 1) = CLAMP_NUM(outGreen, 0, 255);
						*(destPixel + 2) = CLAMP_NUM(outBlue,  0, 255);
						*(destPixel + 3) = CLAMP_NUM(outAlpha, 0, 255);
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
	if (HAS_SUFFIX_CI(filePath, ".csprite", 8)) {
#define WRITE_CHECKED(file, src, szBytes) do { if (fwrite(src, szBytes, 1, file) != 1) { log_error("Cannot write %d bytes", szBytes); return -1; } } while(0)

		FILE* fp = fopen(filePath, "wb");
		if (fp == NULL) {
			log_error("Cannot open the file: %s\n", filePath);
			return -1;
		}

		int32_t numChannels = 4;
		char signature[4] = "DEEZ";
		uint16_t formatVersion = 1; // max uint16_t 65535

		WRITE_CHECKED(fp, signature, 4);

		// htonX function converts the value to a big-endian value
		{ uint16_t b_formatVersion = SWAP_ONLY_BIGE_u16(formatVersion); WRITE_CHECKED(fp, &b_formatVersion, 2); }
		{ int32_t b_w = SWAP_ONLY_BIGE_i32(w); WRITE_CHECKED(fp, &b_w, 4); }
		{ int32_t b_h = SWAP_ONLY_BIGE_i32(h); WRITE_CHECKED(fp, &b_h, 4); }
		{ int32_t b_numChannels = SWAP_ONLY_BIGE_i32(numChannels); WRITE_CHECKED(fp, &b_numChannels, 4); }
		{ int32_t b_numLayers = SWAP_ONLY_BIGE_i32(arr->size); WRITE_CHECKED(fp, &b_numLayers, 4); }
		for (int i = 0; i < arr->size; ++i) {
			WRITE_CHECKED(fp, arr->layers[i]->name, strlen(arr->layers[i]->name) + 1);
		}

		uLongf pixelArrAlignedSize = w * h * numChannels * arr->size * sizeof(uint8_t);
		uLongf dataSizeCompressed = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-size-larger-than"
		uint8_t* pixelArrAligned = (uint8_t*)malloc(pixelArrAlignedSize);
#pragma GCC diagnostic pop
		if (pixelArrAligned == NULL) {
			log_error("Failed to allocate memory buffer to store pixel array aligned");
			fclose(fp);
			return -1;
		}

		uLongf amtCopied = 0;
		for (int i = 0; i < arr->size; ++i) {
			memcpy(pixelArrAligned + amtCopied, arr->layers[i]->pixels, w * h * numChannels);
			amtCopied += w * h * numChannels;
		}

		uint8_t* dataCompressed = Z_CompressData(pixelArrAlignedSize, &dataSizeCompressed, pixelArrAligned);
		if (dataCompressed == NULL || dataSizeCompressed <= 0) {
			log_error("Failed to compress data!");
			fclose(fp);
			return -1;
		}

		WRITE_CHECKED(fp, dataCompressed, dataSizeCompressed);
		free(dataCompressed);
		free(pixelArrAligned);
		dataCompressed = NULL;
		pixelArrAligned = NULL;

		fclose(fp);
		fp = NULL;

#undef WRITE_CHECKED
	} else if (HAS_SUFFIX_CI(filePath, ".png", 4)) {
		_BlendedPixels = BlendPixels_Alpha(w, h, arr);
		if (_BlendedPixels == NULL) {
			log_error("Alpha Blending Failed, BlendPixels_Alpha(...) returned NULL");
			return -1;
		} else {
			stbi_write_png(filePath, w, h, 4, _BlendedPixels, 0);
			free(_BlendedPixels);
			_BlendedPixels = NULL;
		}
	} else if (HAS_SUFFIX_CI(filePath, ".jpeg", 5) || HAS_SUFFIX_CI(filePath, ".jpg", 4)) {
		_BlendedPixels = BlendPixels_Alpha(w, h, arr);
		if (_BlendedPixels == NULL) {
			log_error("Alpha Blending Failed, BlendPixels_Alpha(...) returned NULL");
			return -1;
		} else {
			stbi_write_jpg(filePath, w, h, 4, _BlendedPixels, 100);
			free(_BlendedPixels);
			_BlendedPixels = NULL;
		}
	} else {
		log_error("Error Un-supported file format: %s\n", filePath);
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
			CanvasLayer_T* layer = Canvas_CreateLayer(R_GetRenderer(), w, h);
			*arr = Canvas_CreateArr(100, w, h);
			(*arr)->size++;
			(*arr)->layers[0] = layer;

			memcpy(layer->pixels, _data, w * h * 4);
			memcpy(layer->history->pixels, _data, w * h * 4);
			SaveHistory(&layer->history, w * h * 4, layer->pixels);
			return 0;
		}
	} else if (HAS_SUFFIX_CI(filePath, ".csprite", 8)) {
		int32_t w = 0, h = 0, numChannels = 0, numLayers = 0;
		uint16_t formatVersion = 0;
		char signature[4] = "";
		FILE* fp = fopen(filePath, "rb");
		if (fp == NULL) {
			log_error("Cannot open the file: %s\n", filePath);
			return -1;
		}

		fseek(fp, 0L, SEEK_END);
		uLongf fileSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		if (fileSize < 22) {
			log_error("invalid .csprite file");
			fclose(fp);
			return -1;
		}

		if (fread(signature, 4, 1, fp) != 1) { log_error("failed to read .csprite signature"); fclose(fp); return -1; }
		if (fread(&formatVersion, 2, 1, fp) != 1) { log_error("failed to read .csprite format-version"); fclose(fp); return -1; }
		formatVersion = SWAP_ONLY_BIGE_u16(formatVersion);

		if (strncmp(signature, "DEEZ", 4) != 0 || formatVersion != 1) {
			log_error("invalid .csprite format signature, formatVersion: %d", formatVersion);
			fclose(fp);
			return -1;
		}

		if (fread(&w, 4, 1, fp) != 1) { log_error("failed to read image width"); fclose(fp); return -1; }
		w = SWAP_ONLY_BIGE_i32(w);

		if (fread(&h, 4, 1, fp) != 1) { log_error("failed to read image height"); fclose(fp); return -1; }
		h = SWAP_ONLY_BIGE_i32(h);

		if (fread(&numChannels, 4, 1, fp) != 1) { log_error("failed to read number of channels available"); fclose(fp); return -1; }
		numChannels = SWAP_ONLY_BIGE_i32(numChannels);

		if (fread(&numLayers, 4, 1, fp) != 1) { log_error("failed to read number of layers available"); fclose(fp); return -1; }
		numLayers = SWAP_ONLY_BIGE_i32(numLayers);

		if (w < 1 || h < 1) {
			log_error("invalid width or height, %dx%d", w, h);
			fclose(fp);
			return -1;
		} else if (numChannels != 4) {
			log_error("invalid number of channels: %d, only 4 number of channels is support now", numChannels);
			fclose(fp);
			return -1;
		} else if (numLayers < 1) {
			log_warn("no layers found!");
		}

		Canvas_DestroyArr(*arr);
		*arr = Canvas_CreateArr(numLayers > 100 ? numLayers + 50 : 100, w, h);

		if (numLayers > 0) {
			for (int currLayerIdx = 0; currLayerIdx < numLayers; ++currLayerIdx) {
				char layerName[LAYER_NAME_MAX] = "";
				memset(layerName, '\0', LAYER_NAME_MAX);
				for (int i = 0; i < LAYER_NAME_MAX - 1; ++i) {
					if (fread(&layerName[i], 1, 1, fp) != 1) { log_error("failed to read %d layer's name", currLayerIdx + 1); fclose(fp); return -1; }
					if (layerName[i] == '\0') break;
				}

				CanvasLayer_T* layer = Canvas_CreateLayer(R_GetRenderer(), w, h);
				strncpy(layer->name, layerName, LAYER_NAME_MAX);
				(*arr)->size++;
				(*arr)->layers[currLayerIdx] = layer;
			}

			uLongf compressDataSize = fileSize - ftell(fp);
			uint8_t* compressedData = malloc(compressDataSize);
			if (fread(compressedData, compressDataSize, 1, fp) != 1) { log_error("failed to read compressed data"); fclose(fp); return -1; }

			uLongf originalDataSize = w * h * numChannels * numLayers * sizeof(uint8_t);
			uint8_t* originalData = Z_DeCompressData(compressedData, compressDataSize, originalDataSize);

			int32_t numLayersCopied = 0;
			for (int i = 0; i < (*arr)->size; ++i) {
				memcpy((*arr)->layers[i]->pixels, originalData + ((w * h * numChannels) * numLayersCopied), w * h * numChannels);
				memcpy((*arr)->layers[i]->history->pixels, (*arr)->layers[i]->pixels, w * h * 4 * sizeof(uint8_t));
				SaveHistory(&(*arr)->layers[i]->history, w * h * 4 * sizeof(uint8_t), (*arr)->layers[i]->pixels);
				Canvas_UpdateLayerTexture((*arr)->layers[i], w);
				numLayersCopied++;
			}

			free(compressedData);
			free(originalData);
			compressedData = NULL;
			originalData = NULL;
		}

		*w_ptr = w;
		*h_ptr = h;

		fclose(fp);
		fp = NULL;
		return 0;
	} else {
		log_error("Error Un-supported file format: %s\n", filePath);
	}
	return -1;
}

