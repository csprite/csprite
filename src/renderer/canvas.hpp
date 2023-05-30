#ifndef CSP_RENDERER_CANVAS_HPP_INCLUDED_
#define CSP_RENDERER_CANVAS_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "pixel/pixel.hpp"

struct Canvas {
	using GLuint = unsigned int;
	GLuint id = 0;
	RectF32 viewport = { 0, 0, 0, 0 };

	Canvas(u16 w, u16 h);
	~Canvas();

	void Update(Pixel* pixelData);

private:
	u16 w, h;
};

#endif // CSP_RENDERER_CANVAS_HPP_INCLUDED_
