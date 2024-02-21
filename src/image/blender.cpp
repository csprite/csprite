#include <algorithm> // min()/max()
#include "image/blender.h"

void BlendImage(const Image& img, const mm_RectU32& dirtyArea, Pixel* outBuff, bool checkerboard) {
	for (std::size_t j = 0; j < img.Layers.size(); ++j) {
		for (u32 y = dirtyArea.min_y; y < dirtyArea.max_y; y++) {
			for (u32 x = dirtyArea.min_x; x < dirtyArea.max_x; x++) {
				u32 idx = (y * img.w) + x;
				Pixel& output = outBuff[idx];

				if (j == 0 && checkerboard) {
					#define PATTERN_SCALE 2
					if (((x / PATTERN_SCALE + y / PATTERN_SCALE) % 2) == 0) {
						output = { 0x80, 0x80, 0x80, 255 };
					} else {
						output = { 0xC0, 0xC0, 0xC0, 255 };
					}
					#undef PATTERN_SCALE
				}

				const Layer& layer = img.Layers[j];
				const Pixel src = layer.pixels[idx];
				const Pixel backdrop = output;

				if (src.a == 0 || layer.opacity == 0)
					continue;

				output = BlendPixel(layer.blend, src, backdrop);
				output = BlendAlpha(output, backdrop, layer.opacity);
			}
		}
	}
}

// From pixman-combine32.h -> https://gitlab.freedesktop.org/pixman/pixman/-/blob/fdd716109787ef825f9eb88f73447297c43e5c10/pixman/pixman-combine32.h
#define MUL_UN8(a, b, t) ((t) = (a) * (uint16_t)(b) + 0x80, ((((t) >> 8) + (t)) >> 8))

Pixel BlendAlpha(Pixel src, Pixel backdrop, u8 opacity) {
	if (src.a == 255) return src;
	else if (src.a == 0 || opacity == 0) return backdrop;

	i32 t;
	src.a = MUL_UN8(src.a, opacity, t);

	Pixel out;
	out.a = src.a + backdrop.a - MUL_UN8(src.a, backdrop.a, t);
	out.r = backdrop.r + (src.r - backdrop.r) * src.a / out.a;
	out.g = backdrop.g + (src.g - backdrop.g) * src.a / out.a;
	out.b = backdrop.b + (src.b - backdrop.b) * src.a / out.a;

	return out;
}

Pixel BlendPixel(Blend mode, Pixel src, Pixel backdrop) {
	i32 r = 0, g = 0, b = 0, t;

	switch (mode) {
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

	Pixel out;
	out.r = MIN_MAX(r, 0, 255);
	out.g = MIN_MAX(g, 0, 255);
	out.b = MIN_MAX(b, 0, 255);
	out.a = src.a;
	return out;
}
