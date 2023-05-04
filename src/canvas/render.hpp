#ifndef CSP_CANVAS_RENDER_HPP_INCLUDED_
#define CSP_CANVAS_RENDER_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "pixel/pixel.hpp"
#include "window/renderer.hpp"

// Functions & Helpers For Rendering Bitmap To Screen

namespace Canvas {
	struct Render {
		Renderer::Texture* texture;
		Pixel* pixels;
		i32 pitch;

		Render(u16 width, u16 height);
		~Render();
	};

	void Draw(Render& finalRender, Rect& container, Rect& dirtyArea); // draws the bitmap to screen in the "container"
}

#endif // CSP_CANVAS_RENDER_HPP_INCLUDED_
