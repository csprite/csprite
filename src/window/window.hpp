#include <SDL_video.h>
#ifndef CSP_WINDOW_WINDOW_HPP_INCLUDED_
#define CSP_WINDOW_WINDOW_HPP_INCLUDED_ 1

#include <SDL.h>
#include "types.hpp"

using Window = SDL_Window;

Window* AppWindow_Init(u32 width, u32 height);
Window* AppWindow_Get();
void AppWindow_Destroy();

#endif // CSP_WINDOW_WINDOW_HPP_INCLUDED_
