#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "log/log.h"
#include "./renderer.h"

static SDL_Renderer* renderer = NULL;

SDL_Renderer* R_GetRenderer() {
	return renderer;
}

int R_Init(SDL_Window* window) {
	uint32_t renFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;

	renderer = SDL_CreateRenderer(window, -1, renFlags);
	if (renderer == NULL) {
		log_error("Failed to create SDL_Renderer, SDL_CreateRenderer() returned NULL!");
		return EXIT_FAILURE;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return EXIT_SUCCESS;
}

void R_Destroy(void) {
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	renderer = NULL;
}

void R_Clear(void) {
	SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
	SDL_RenderClear(renderer);
}

void R_NewFrame(void) {
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void R_Present(void) {
	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(renderer); // Swap Front & Back Buffers
}

