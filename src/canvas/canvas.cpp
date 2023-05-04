#include "doc/bitmap.hpp"
#include "pixel/pixel.hpp"
#include "types.hpp"
#include "canvas/canvas.hpp"
#include "canvas/render.hpp"

Rect container = {
	100, 100, 32*7, 32*7
};

#define BLEND_NORMAL(A, B)        ((u8)(A))
#define BLEND_SUBTRACT(A, B)      ((u8)((A + B < 255) ? 0:(A + B - 255)))
#define BLEND_ALPHA(A,B,O)        ((u8)(O * A + (1 - O) * B))
#define BLEND_ALPHA_F(A, B, F, O) (BLEND_ALPHA(F(A,B),A,O))

void Canvas::Blend(Bitmap::Bitmap &bitmap, Rect &dirtyArea) {
	for (u32 i = 0; i < bitmap.layers.size(); ++i) {
		const Bitmap::BitmapLayer& layer = bitmap.layers[i];
		const Pixel* pixelsFront = layer.pixels;
		Pixel* pixelsBehind = bitmap.finalRender.pixels;

		for (u16 y = dirtyArea.y; y < dirtyArea.h; y++) {
			for (u16 x = dirtyArea.x; x < dirtyArea.w; x++) {
				const Pixel* front = pixelsFront + ((y * bitmap.width) + x);
				Pixel* back = pixelsBehind + ((y * bitmap.width) + x);

				pixelsBehind->r = BLEND_NORMAL(front->r, pixelsBehind->r);
				pixelsBehind->g = BLEND_NORMAL(front->g, pixelsBehind->g);
				pixelsBehind->b = BLEND_NORMAL(front->b, pixelsBehind->b);
				pixelsBehind->a = BLEND_ALPHA_F(front->a, pixelsBehind->a, BLEND_SUBTRACT, layer.opacity);
			}
		}
	}

	Canvas::Draw(bitmap.finalRender, container, dirtyArea);
}
