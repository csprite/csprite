#include <cstring>
#include <cmath>
#include <algorithm> // min()/max()
#include "image/blender.hpp"

void Blender::Blend(const Image& img, const RectU32& dirtyArea, Pixel* outBuff, bool checkerboard) {
	bool firstPass = true && checkerboard;

	for (std::size_t j = 0; j < img.Layers.size(); ++j) {
		for (u32 y = dirtyArea.y; y < dirtyArea.h; y++) {
			for (u32 x = dirtyArea.x; x < dirtyArea.w; x++) {
				i32 idx = (y * img.w) + x;
				Pixel& frontPixel = img.Layers[j].pixels[idx];
				Pixel& backPixel = outBuff[idx];

				if (firstPass) {
					// checkerboard pattern
					#define PATTERN_SCALE 2
					if (((x / PATTERN_SCALE + y / PATTERN_SCALE) % 2) == 0) {
						backPixel = { 0x80, 0x80, 0x80, 255 };
					} else {
						backPixel = { 0xC0, 0xC0, 0xC0, 255 };
					}
				}

				if (frontPixel.a == 0) {
					continue; // Keep `backPixel` as is, if nothing to blend with in front
				} else {
					i32 r = 0, g = 0, b = 0, a = 0;
					switch (img.Layers[j].blend) {
						case Alpha: {
							r = frontPixel.r;
							g = frontPixel.g;
							b = frontPixel.b;
							break;
						}
						case Addition: {
							r = frontPixel.r + backPixel.r;
							g = frontPixel.g + backPixel.g;
							b = frontPixel.b + backPixel.b;
							break;
						}
						case Subtraction: {
							r = frontPixel.r - backPixel.r;
							g = frontPixel.g - backPixel.g;
							b = frontPixel.b - backPixel.b;
							break;
						}
						case Difference: {
							r = std::abs(frontPixel.r - backPixel.r);
							g = std::abs(frontPixel.g - backPixel.g);
							b = std::abs(frontPixel.b - backPixel.b);
							break;
						}
						case Multiply: {
							r = ((frontPixel.r / 255.0f) * (backPixel.r / 255.0f)) * 255.0f;
							g = ((frontPixel.g / 255.0f) * (backPixel.g / 255.0f)) * 255.0f;
							b = ((frontPixel.b / 255.0f) * (backPixel.b / 255.0f)) * 255.0f;
							break;
						}
						case Screen: {
							r = (backPixel.r + frontPixel.r - (((frontPixel.r/255) * (backPixel.r/255)) * 255));
							g = (backPixel.g + frontPixel.g - (((frontPixel.g/255) * (backPixel.g/255)) * 255));
							b = (backPixel.b + frontPixel.b - (((frontPixel.b/255) * (backPixel.b/255)) * 255));
							break;
						}
						case Darken: {
							r = std::min(backPixel.r, frontPixel.r);
							g = std::min(backPixel.g, frontPixel.g);
							b = std::min(backPixel.b, frontPixel.b);
							break;
						}
						case Lighten: {
							r = std::max(backPixel.r, frontPixel.r);
							g = std::max(backPixel.g, frontPixel.g);
							b = std::max(backPixel.b, frontPixel.b);
							break;
						}
					}

					// Normal Blending
					if (frontPixel.a < 255) {
						r = (r * frontPixel.a + (i32)backPixel.r * (255 - frontPixel.a) / 255 * backPixel.a) / 255;
						g = (g * frontPixel.a + (i32)backPixel.g * (255 - frontPixel.a) / 255 * backPixel.a) / 255;
						b = (b * frontPixel.a + (i32)backPixel.b * (255 - frontPixel.a) / 255 * backPixel.a) / 255;
					}
					a = frontPixel.a + (i32)backPixel.a * (255 - frontPixel.a) / 255;

					backPixel.r = CLAMP_NUM_TO_TYPE(r, u8);
					backPixel.g = CLAMP_NUM_TO_TYPE(g, u8);
					backPixel.b = CLAMP_NUM_TO_TYPE(b, u8);
					backPixel.a = CLAMP_NUM_TO_TYPE(a, u8);
				}
			}
		}
		firstPass = false;
	}
}

