#ifndef CSP_DOC_BITMAP_HPP_INCLUDED_
#define CSP_DOC_BITMAP_HPP_INCLUDED_ 1
#pragma once

// Functions & Helpers For Managing, Blending & Drawing Bitmaps

#include "types.hpp"
#include "fs/fs_std.hpp"
#include "pixel/pixel.hpp"
#include "window/renderer.hpp"

struct Bitmap {
private:
	Renderer::Texture* _gpuTex;
	f32 scaleX;
	f32 scaleY;
	void GenerateTexture();
	void UpdateTexture();

public:
	u16 width;
	u16 height;
	fs::path filePath;
	Pixel* pixels;

	/* Bounding Box specifies where the image is on screen at x, y position
	   and specifies the widht & height of the box, changes to the member
	   shall only be done via SetBoundingBox() function */
	Rect bBox;

	Bitmap(fs::path filePath);
	Bitmap(u16 w, u16 h);
	~Bitmap();

	void SetBoundingBox(const Rect& newBox);
	void Draw(const Rect& dirtyArea);
};

#endif // CSP_DOC_BITMAP_HPP_INCLUDED_
