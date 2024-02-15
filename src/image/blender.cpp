#include <cstring>
#include <cmath>
#include <algorithm> // min()/max()
#include "image/blender.hpp"

// From pixman-combine32.h -> https://gitlab.freedesktop.org/pixman/pixman/-/blob/fdd716109787ef825f9eb88f73447297c43e5c10/pixman/pixman-combine32.h
#define ONE_HALF 0x80
// Same as `((a/255) * (b/255)) * 255` But FAST
#define MUL_UN8(a, b, t) ((t) = (a) * (uint16_t)(b) + ONE_HALF, ((((t) >> 8 ) + (t) ) >> 8 ))

void Blender::Blend(const Image& img, const RectU32& dirtyArea, Pixel* outBuff, bool checkerboard) {
	bool firstPass = true && checkerboard;

	for (std::size_t j = 0; j < img.Layers.size(); ++j) {
		for (u32 y = dirtyArea.y; y < dirtyArea.h; y++) {
			for (u32 x = dirtyArea.x; x < dirtyArea.w; x++) {
				i32 idx = (y * img.w) + x;
				Pixel& output = outBuff[idx];

				if (firstPass) {
					// checkerboard pattern
					#define PATTERN_SCALE 2
					if (((x / PATTERN_SCALE + y / PATTERN_SCALE) % 2) == 0) {
						output = { 0x80, 0x80, 0x80, 255 };
					} else {
						output = { 0xC0, 0xC0, 0xC0, 255 };
					}
				}

				const Layer& layer = img.Layers[j];
				const Pixel backdrop = output;
				Pixel src = layer.pixels[idx];

				if (src.a == 0 || layer.opacity == 0) {
					continue; // Keep `output` as is, if nothing to blend with in front
				} else {
					i32 r = 0, g = 0, b = 0;
					int t;
					switch (img.Layers[j].blend) {
						case Normal: {
							r = src.r;
							g = src.g;
							b = src.b;
							break;
						}
						case Addition: {
							r = src.r + backdrop.r;
							g = src.g + backdrop.g;
							b = src.b + backdrop.b;
							break;
						}
						case Subtraction: {
							r = src.r - backdrop.r;
							g = src.g - backdrop.g;
							b = src.b - backdrop.b;
							break;
						}
						case Difference: {
							r = std::abs(src.r - backdrop.r);
							g = std::abs(src.g - backdrop.g);
							b = std::abs(src.b - backdrop.b);
							break;
						}
						case Multiply: {
							r = MUL_UN8(src.r, backdrop.r, t);
							g = MUL_UN8(src.g, backdrop.g, t);
							b = MUL_UN8(src.b, backdrop.b, t);
							break;
						}
						case Screen: {
							r = backdrop.r + src.r - (MUL_UN8(src.r, backdrop.r, t));
							g = backdrop.g + src.g - (MUL_UN8(src.g, backdrop.g, t));
							b = backdrop.b + src.b - (MUL_UN8(src.b, backdrop.b, t));
							break;
						}
						case Darken: {
							r = std::min(backdrop.r, src.r);
							g = std::min(backdrop.g, src.g);
							b = std::min(backdrop.b, src.b);
							break;
						}
						case Lighten: {
							r = std::max(backdrop.r, src.r);
							g = std::max(backdrop.g, src.g);
							b = std::max(backdrop.b, src.b);
							break;
						}
						case Count: break;
					}

					int a;

					// Normal Blending
					src.a = MUL_UN8(src.a, layer.opacity, t); // "Blend" Pixel & Layer Alpha
					if (src.a < 255) {
						r = (r * src.a + (i32)backdrop.r * (255 - src.a) / 255 * backdrop.a) / 255;
						g = (g * src.a + (i32)backdrop.g * (255 - src.a) / 255 * backdrop.a) / 255;
						b = (b * src.a + (i32)backdrop.b * (255 - src.a) / 255 * backdrop.a) / 255;
						a = src.a + (i32)backdrop.a * (255 - src.a) / 255;
					} else {
						a = 255;
					}

					output.r = CLAMP_NUM_TO_TYPE(r, u8);
					output.g = CLAMP_NUM_TO_TYPE(g, u8);
					output.b = CLAMP_NUM_TO_TYPE(b, u8);
					output.a = CLAMP_NUM_TO_TYPE(a, u8);
				}
			}
		}
		firstPass = false;
	}
}

