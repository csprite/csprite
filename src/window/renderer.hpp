#include <SDL_render.h>
#ifndef CSP_WINDOW_RENDERER_HPP_INCLUDED_
#define CSP_WINDOW_RENDERER_HPP_INCLUDED_ 1

#include <SDL.h>
#include "window/window.hpp"

namespace Renderer {
	using Renderer = SDL_Renderer;

	Renderer* Init(Window::Window* win);
	Renderer* Get();
	void NewFrame();
	void Render();
	void Destroy();
}

#endif // CSP_WINDOW_RENDERER_HPP_INCLUDED_
