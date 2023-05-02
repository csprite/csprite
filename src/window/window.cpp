#include <SDL.h>
#include <SDL_error.h>
#include <SDL_main.h>
#include <SDL_timer.h>
#include <SDL_video.h>

#include "log.hpp"
#include "types.hpp"
#include "window/window.hpp"
#include "window/renderer.hpp"

static Window::Window* win = NULL;
static Renderer::Renderer* ren = NULL;

Window::Window* Window::Init(u32 width, u32 height) {
	SDL_SetMainReady();

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

	Renderer::Init(win);

	SDL_ShowWindow(win);
	return win;
}

Window::Window* Window::Get() {
	return win;
}

void Window::ProcessEvents() {
	bool ShouldClose = false;
	SDL_Event event;
	u32 frameStart = SDL_GetTicks(), // needs to be initialzed with some starting value or SDL_Delay will sleep for seconds passed since Unix Epoch
		frameTime = 0,
		frameDelay = 1000 / 50;

	while (!ShouldClose) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					ShouldClose = true;
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
						ShouldClose = true;
					}
					break;
			}
		}

		Renderer::NewFrame();

		Renderer::Render();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameTime > frameStart) SDL_Delay(frameDelay - frameTime);
		frameStart = SDL_GetTicks();
	}
}

void Window::Destroy() {
	if (win != NULL) {
		Renderer::Destroy();
		SDL_DestroyWindow(win);
		SDL_Quit();
		win = NULL;
	}
}
