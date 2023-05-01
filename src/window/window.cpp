#include <SDL.h>
#include <SDL_error.h>
#include <SDL_video.h>

#include "log.hpp"
#include "window/window.hpp"

Window* win = NULL;

Window* AppWindow_Init(u32 width, u32 height) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
		LOG_E("Failed to initialize SDL2: %s", SDL_GetError());
		return NULL;
	}

	win = SDL_CreateWindow(
		"csprite",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
	);
	if (win == NULL) {
		LOG_E("Failed to create a window: %s", SDL_GetError());
		SDL_Quit();
		return NULL;
	}

	SDL_ShowWindow(win);
	return win;
}

Window* AppWindow_Get() {
	return win;
}

void AppWindow_Destroy() {
	if (win != NULL) {
		SDL_DestroyWindow(win);
		SDL_Quit();
		win = NULL;
	}
}
