#ifndef CSP_DOC_BITMAP_HPP_INCLUDED_
#define CSP_DOC_BITMAP_HPP_INCLUDED_ 1
#pragma once

// Functions & Helpers For Managing Bitmaps

#include "types.hpp"
#include "pixel/pixel.hpp"
#include "window/renderer.hpp"

struct Bitmap {
	u16 width;
	u16 height;
	Pixel* pixels;
	Bitmap(u16 w, u16 h);
	~Bitmap();
	void Draw(const Rect& OutputContainer, const Rect& dirtyArea);

private:
	Renderer::Texture* _gpuTex;
};

#endif // CSP_DOC_BITMAP_HPP_INCLUDED_
