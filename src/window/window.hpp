#include <SDL_events.h>
#include <SDL_video.h>
#ifndef CSP_WINDOW_WINDOW_HPP_INCLUDED_
#define CSP_WINDOW_WINDOW_HPP_INCLUDED_ 1
#pragma once

#include <SDL.h>
#include "types.hpp"

namespace Window {
	using Window = SDL_Window;
	using Event = SDL_Event;

	Window* Init(u32 width, u32 height);
	Window* Get();
	void ProcessEvents();
	void Destroy();
}

#endif // CSP_WINDOW_WINDOW_HPP_INCLUDED_
