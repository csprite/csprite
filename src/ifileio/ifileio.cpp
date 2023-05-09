#include "log/log.h"
#include "../utils.h"
#include "../renderer/renderer.h"
#include "../renderer/canvas.h"

#include "ifileio.h"
#include "ifileio_endian.h"
#include "zlib_wrapper.h"

#include "stb_image.h"
#include "stb_image_write.h"
#include <algorithm>

static Pixel* BlendPixels_Alpha(CanvasLayer_Manager* mgr) {
	int32_t w = mgr->dims[0], h = mgr->dims[1];
	Pixel* blendedPixels = new Pixel[w * h];

	for (uint32_t i = 0; i < mgr->layers.size(); ++i) {
		for (int32_t y = 0; y < h; ++y) {
			for (int32_t x = 0; x < w; ++x) {
				// Simple Alpha-Blending Being Done Here.
				Pixel& srcPixel = mgr->layers[i]->pixels[(y * w) + x];
				Pixel& destPixel = blendedPixels[(y * w) + x];

				#define CLAMP_u16_to_u8(val) (u8)std::clamp((val), u16(0), u16(255))
				destPixel = {
					.r = CLAMP_u16_to_u8(static_cast<u16>(
						((uint16_t)srcPixel.r * srcPixel.a + (uint16_t)destPixel.r * (255 - srcPixel.a) / 255 * destPixel.a) / 255
					)),
					.g = CLAMP_u16_to_u8(static_cast<u16>(
						((uint16_t)srcPixel.g * srcPixel.a + (uint16_t)destPixel.g * (255 - srcPixel.a) / 255 * destPixel.a) / 255
					)),
					.b = CLAMP_u16_to_u8(static_cast<u16>(
						((uint16_t)srcPixel.b * srcPixel.a + (uint16_t)destPixel.b * (255 - srcPixel.a) / 255 * destPixel.a) / 255
					)),
					.a = CLAMP_u16_to_u8(static_cast<u16>(
						srcPixel.a + (uint16_t)destPixel.a * (255 - srcPixel.a) / 255
					)),
				};
				#undef CLAMP_u16_to_u8
			}
		}
	}
	return blendedPixels;
}

int32_t ifio_write(const char* filePath, CanvasLayer_Manager* mgr) {
	if (filePath == NULL || mgr == NULL || mgr->dims[0] < 1 || mgr->dims[1] < 1) return -1;
	int32_t w = mgr->dims[0], h = mgr->dims[1];

	Pixel* _BlendedPixels = NULL;
	if (HAS_SUFFIX_CI(filePath, ".csprite", 8)) {
#define WRITE_CHECKED(file, src, szBytes) do { if (fwrite(src, szBytes, 1, file) != 1) { log_error("Cannot write %d bytes", szBytes); return -1; } } while(0)

		FILE* fp = fopen(filePath, "wb");
		if (fp == NULL) {
			log_error("Cannot open the file: %s\n", filePath);
			return -1;
		}

		int32_t numChannels = 4;
		char signature[5] = "DEEZ";
		uint16_t formatVersion = 1; // max uint16_t 65535

		WRITE_CHECKED(fp, signature, 4);

		// htonX function converts the value to a big-endian value
		{ uint16_t b_formatVersion = SWAP_ONLY_BIGE_u16(formatVersion); WRITE_CHECKED(fp, &b_formatVersion, 2); }
		{ int32_t b_w = SWAP_ONLY_BIGE_i32(w); WRITE_CHECKED(fp, &b_w, 4); }
		{ int32_t b_h = SWAP_ONLY_BIGE_i32(h); WRITE_CHECKED(fp, &b_h, 4); }
		{ int32_t b_numChannels = SWAP_ONLY_BIGE_i32(numChannels); WRITE_CHECKED(fp, &b_numChannels, 4); }
		{ int32_t b_numLayers = SWAP_ONLY_BIGE_i32(mgr->layers.size()); WRITE_CHECKED(fp, &b_numLayers, 4); }
		for (int i = 0; i < mgr->layers.size(); ++i) {
			WRITE_CHECKED(fp, mgr->layers[i]->name.c_str(), mgr->layers[i]->name.length() + 1);
		}

		uLongf pixelArrAlignedSize = w * h * numChannels * mgr->layers.size() * sizeof(uint8_t);
		uLongf dataSizeCompressed = 0;
		uint8_t* pixelArrAligned = (uint8_t*)malloc(pixelArrAlignedSize);
		if (pixelArrAligned == NULL) {
			log_error("Failed to allocate memory buffer to store pixel array aligned");
			fclose(fp);
			return -1;
		}

		uLongf amtCopied = 0;
		for (int i = 0; i < mgr->layers.size(); ++i) {
			memcpy(pixelArrAligned + amtCopied, mgr->layers[i]->pixels, w * h * numChannels);
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
		_BlendedPixels = BlendPixels_Alpha(mgr);
		if (_BlendedPixels == NULL) {
			log_error("Alpha Blending Failed, BlendPixels_Alpha(...) returned NULL");
			return -1;
		} else {
			stbi_write_png(filePath, w, h, 4, _BlendedPixels, 0);
			delete[] _BlendedPixels;
			_BlendedPixels = NULL;
		}
	} else if (HAS_SUFFIX_CI(filePath, ".jpeg", 5) || HAS_SUFFIX_CI(filePath, ".jpg", 4)) {
		_BlendedPixels = BlendPixels_Alpha(mgr);
		if (_BlendedPixels == NULL) {
			log_error("Alpha Blending Failed, BlendPixels_Alpha(...) returned NULL");
			return -1;
		} else {
			stbi_write_jpg(filePath, w, h, 4, _BlendedPixels, 100);
			delete[] _BlendedPixels;
			_BlendedPixels = NULL;
		}
	} else {
		log_error("Error Un-supported file format: %s\n", filePath);
		return -1;
	}

	return 0;
}

int32_t ifio_read(const char* filePath, CanvasLayer_Manager** mgr_ptr, uint8_t checkBg1[4], uint8_t checkBg2[4]) {
	if (filePath == NULL || mgr_ptr == NULL || *mgr_ptr == NULL) return -1;

	if (HAS_SUFFIX_CI(filePath, ".png", 4) || HAS_SUFFIX_CI(filePath, ".jpeg", 5) || HAS_SUFFIX_CI(filePath, ".jpg", 4)) {
		int32_t w = 0, h = 0, channels = 0;
		uint8_t* _data = stbi_load(filePath, &w, &h, &channels, 4);
		if (w > 0 && h > 0 && _data) {
			CanvasLayer_Manager* mgr = new CanvasLayer_Manager((*mgr_ptr)->ren, w, h, checkBg1, checkBg2);
			mgr->AddLayer();
			mgr->SetCurrentLayerIdx(0);
			CanvasLayer* layer = mgr->layer;
			memcpy(layer->pixels, _data, w * h * 4);
			memcpy(layer->history->pixels, _data, w * h * 4);
			SaveHistory(&layer->history, w * h * 4, layer->pixels);
			free(_data);
			delete (*mgr_ptr);
			*mgr_ptr = mgr;
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

		CanvasLayer_Manager* mgr = new CanvasLayer_Manager((*mgr_ptr)->ren, w, h, checkBg1, checkBg2);

		if (numLayers > 0) {
			for (int currLayerIdx = 0; currLayerIdx < numLayers; ++currLayerIdx) {
				mgr->AddLayer();
				CanvasLayer* layer = mgr->layers[currLayerIdx];
				int32_t len = 0; // string length with null character
				char c = 'a';
				for (len = 0; c != '\0'; ++len) {
					if (fread(&c, 1, 1, fp) != 1) { log_error("failed to read %d layer's name", currLayerIdx + 1); fclose(fp); delete mgr; return -1; }
				}
				fseek(fp, -len, SEEK_CUR);
				layer->name.clear();
				layer->name.reserve(len);
				for (int32_t i = 0; i < len; ++i) {
					if (fread(&c, 1, 1, fp) != 1) { log_error("failed to read %d layer's name", currLayerIdx + 1); fclose(fp); delete mgr; return -1; }
					if (c == '\0') break;
					layer->name.append(1, c);
				}
			}
			mgr->SetCurrentLayerIdx(0);

			uLongf compressDataSize = fileSize - ftell(fp);
			uint8_t* compressedData = (uint8_t*)malloc(compressDataSize);
			if (fread(compressedData, compressDataSize, 1, fp) != 1) { log_error("failed to read compressed data"); fclose(fp); delete mgr; return -1; }

			uLongf originalDataSize = w * h * numChannels * numLayers;
			uint8_t* originalData = Z_DeCompressData(compressedData, compressDataSize, originalDataSize);
			if (originalData == NULL) {
				log_error("failed to decompress the data, Z_DeCompressData(...) returned NULL!");
				free(compressedData);
				fclose(fp);
				delete mgr;
				return -1;
			}

			int32_t numLayersCopied = 0;
			for (int i = 0; i < mgr->layers.size(); ++i) {
				// if (mgr->layers[i]->pixels == NULL) return -1;
				memcpy(mgr->layers[i]->pixels, originalData + ((w * h * numChannels) * numLayersCopied), w * h * numChannels);
				SaveHistory(&mgr->layers[i]->history, w * h * 4, mgr->layers[i]->pixels);
				// memcpy(mgr->layers[i]->history->pixels, mgr->layers[i]->pixels, w * h * 4);
				mgr->ReUploadTexture(i);
				numLayersCopied++;
			}

			free(compressedData);
			free(originalData);
			compressedData = NULL;
			originalData = NULL;
		}

		delete *mgr_ptr;
		*mgr_ptr = mgr;

		fclose(fp);
		fp = NULL;
		return 0;
	} else {
		log_error("Error Un-supported file format: %s\n", filePath);
	}
	return -1;
}
