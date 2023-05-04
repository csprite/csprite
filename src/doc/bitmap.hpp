#ifndef CSP_DOC_BITMAP_HPP_INCLUDED_
#define CSP_DOC_BITMAP_HPP_INCLUDED_ 1
#pragma once

// Functions & Helpers For Managing Bitmaps

#include "types.hpp"
#include "pixel/pixel.hpp"
#include "canvas/render.hpp"

namespace Bitmap {
	enum BlendMode {
		NORMAL = 0
	};

	struct BitmapLayer {
		String name;
		Pixel* pixels;
		BlendMode bMode;
		f32 opacity;

		BitmapLayer(String& _name, u16 _w, u16 _h, f32 _transparency);
		~BitmapLayer();
	};

	struct Bitmap {
		u16 width;
		u16 height;
		Canvas::Render finalRender;
		Vector<BitmapLayer> layers;
		Bitmap(u16 w, u16 h);
	};
}

#endif // CSP_DOC_BITMAP_HPP_INCLUDED_
