#ifndef CSP_RENDERER_CANVAS_HPP_INCLUDED_
#define CSP_RENDERER_CANVAS_HPP_INCLUDED_ 1
#pragma once

#include "glad/glad.h"
#include "types.hpp"
#include "pixel/pixel.hpp"

/* Functions For Canvas Rendering.
    - The canvas is basically the final rendered image that is shown to the screen,
	  
*/

struct Canvas {
	using TextureID = GLuint;
	TextureID id = 0;
	Rect viewPort = { 0, 0, 0, 0 };

	Canvas(u16 w, u16 h);
	~Canvas();

	void Update(Rect& dirtyArea, Pixel* pixelData);
};

namespace CanvasRenderer {
	void Init();
	void Draw(Canvas& c);
	void Release();
}

#endif // CSP_RENDERER_CANVAS_HPP_INCLUDED_
