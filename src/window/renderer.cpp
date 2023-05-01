#include <SDL_error.h>
#include <SDL_render.h>

#include "log.hpp"
#include "window/renderer.hpp"

static RendererNS::Renderer* ren = NULL;

RendererNS::Renderer* RendererNS::Init(WindowNS::Window* win) {
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (ren == NULL) {
		LOG_E("Failed to initialize renderer: %s", SDL_GetError());
		return NULL;
	}

	return ren;
}

RendererNS::Renderer* RendererNS::Get() {
	return ren;
}

void RendererNS::NewFrame() {
	SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
	SDL_RenderClear(ren);
}

void RendererNS::Render() {
	SDL_RenderPresent(ren);
}

void RendererNS::Destroy() {
	if (ren) {
		SDL_DestroyRenderer(ren);
		ren = NULL;
	}
}

