#ifndef CSP_RENDERER_CANVAS_HPP_INCLUDED_
#define CSP_RENDERER_CANVAS_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "imgui/imgui.h"
#include "glad/glad.h"
#include "pixel/pixel.hpp"

/* Functions For Canvas Rendering.
    - The canvas is basically the final rendered image that is shown to the screen,
	  
*/

struct Canvas {
	using TextureID = GLuint;
	TextureID id = 0;
	RectF32 viewport = { 0, 0, 0, 0 };

	Canvas(u16 w, u16 h);
	~Canvas();

	void Update(RectI32& dirtyArea, Pixel* pixelData);
};

#endif // CSP_RENDERER_CANVAS_HPP_INCLUDED_
