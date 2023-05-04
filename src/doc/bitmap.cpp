#include "doc/bitmap.hpp"
#include "pixel/pixel.hpp"

Bitmap::BitmapLayer::BitmapLayer(String& _name, u16 _w, u16 _h, f32 _transparency) {
	name = _name;
	pixels = new Pixel[_w * _h];
	opacity = _transparency;
	bMode = BlendMode::NORMAL;
}

Bitmap::BitmapLayer::~BitmapLayer() {
	delete[] pixels;
}

Bitmap::Bitmap::Bitmap(u16 w, u16 h) : finalRender(w, h) {
	width = w;
	height = h;
}

