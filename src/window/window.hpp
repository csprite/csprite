#include <SDL_video.h>
#ifndef CSP_WINDOW_WINDOW_HPP_INCLUDED_
#define CSP_WINDOW_WINDOW_HPP_INCLUDED_ 1

#include <SDL.h>
#include "types.hpp"

namespace WindowNS {
	using Window = SDL_Window;

	Window* Init(u32 width, u32 height);
	Window* Get();
	void ProcessEvents();
	void Destroy();
}

#endif // CSP_WINDOW_WINDOW_HPP_INCLUDED_
