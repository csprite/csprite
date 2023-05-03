#include <SDL_render.h>
#ifndef CSP_WINDOW_RENDERER_HPP_INCLUDED_
#define CSP_WINDOW_RENDERER_HPP_INCLUDED_ 1
#pragma once

#include <SDL.h>
#include "window/window.hpp"

namespace Renderer {
	using Renderer = SDL_Renderer;
	using Texture = SDL_Texture;

	Renderer* Init(Window::Window* win);
	Renderer* Get();
	void NewFrame();
	void Render();
	void Destroy();
}

#endif // CSP_WINDOW_RENDERER_HPP_INCLUDED_
