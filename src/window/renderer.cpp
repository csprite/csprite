#include <SDL_error.h>
#include <SDL_render.h>

#include "log.hpp"
#include "window/renderer.hpp"

static Renderer* ren = NULL;

Renderer* AppRenderer_Init(Window* win) {
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (ren == NULL) {
		LOG_E("Failed to initialize renderer: %s", SDL_GetError());
		return NULL;
	}

	return ren;
}

Renderer* AppRenderer_Get() {
	return ren;
}

void AppRenderer_Destroy() {
	if (ren) {
		SDL_DestroyRenderer(ren);
		ren = NULL;
	}
}

