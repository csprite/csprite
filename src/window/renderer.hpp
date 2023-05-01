#include <SDL_render.h>
#ifndef CSP_WINDOW_RENDERER_HPP_INCLUDED_
#define CSP_WINDOW_RENDERER_HPP_INCLUDED_ 1

#include <SDL.h>
#include "window/window.hpp"

using Renderer = SDL_Renderer;

Renderer* AppRenderer_Init(Window* win);
Renderer* AppRenderer_Get();
void AppRenderer_Destroy();

#endif // CSP_WINDOW_RENDERER_HPP_INCLUDED_
