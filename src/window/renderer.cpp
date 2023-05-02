#include <SDL_error.h>
#include <SDL_render.h>

#include "log.hpp"
#include "window/gui.hpp"
#include "window/renderer.hpp"

static Renderer::Renderer* ren = NULL;

Renderer::Renderer* Renderer::Init(Window::Window* win) {
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (ren == NULL) {
		LOG_E("Failed to initialize renderer: %s", SDL_GetError());
		return NULL;
	}

	return ren;
}

Renderer::Renderer* Renderer::Get() {
	return ren;
}

void Renderer::NewFrame() {
	SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
	SDL_RenderClear(ren);
    SDL_RenderSetScale(ren, Gui::GetDisplayFbScaleX(), Gui::GetDisplayFbScaleY());
}

void Renderer::Render() {
	SDL_RenderPresent(ren);
}

void Renderer::Destroy() {
	if (ren) {
		SDL_DestroyRenderer(ren);
		ren = NULL;
	}
}

