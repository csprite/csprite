#include <algorithm>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "log/log.h"
#include "./renderer.h"

static SDL_Renderer* renderer = NULL;

SDL_Renderer* R_GetRenderer() {
	return renderer;
}

std::string R_RendererApiToString(Renderer_API api) {
	switch (api) {
		case R_API_DIRECT3D:  return "direct3d";  break;
		case R_API_OPENGL:    return "opengl";    break;
		case R_API_OPENGLES2: return "opengles2"; break;
		case R_API_OPENGLES:  return "opengles";  break;
		case R_API_METAL:     return "metal";     break;
		case R_API_SOFTWARE:  return "software";  break;
		default:              return ""; break;
	}
}

// Todo: Move this to into utils
static void StringToLower(std::string& str) {
	std::transform(
		str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::tolower(c); }
	);
}

Renderer_API R_StringToRendererApi(std::string str) {
	StringToLower(str);
	if (str == "direct3d")       return R_API_DIRECT3D;
	else if (str == "opengl")    return R_API_OPENGL;
	else if (str == "opengles2") return R_API_OPENGLES2;
	else if (str == "opengles")  return R_API_OPENGLES;
	else if (str == "metal")     return R_API_METAL;
	else if (str == "software")  return R_API_SOFTWARE;

	return R_API_INVALID;
}

int R_Init(SDL_Window* window, Renderer_API api) {
	if (api < 0 || api >= R_API_COUNT) {
		log_info("Invalid 'Renderer_API' value: %d", api);
	}

	SDL_SetHint(SDL_HINT_RENDER_DRIVER, R_RendererApiToString(api).c_str());
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (renderer == NULL) {
		log_error("Failed to create SDL_Renderer, SDL_CreateRenderer() returned NULL!");
		return -1;
	}

	SDL_RendererInfo rendererInfo;
	SDL_GetRendererInfo(renderer, &rendererInfo);
	if (R_RendererApiToString(api) == rendererInfo.name) {
		log_info("Initialized renderer with %s render driver!", R_RendererApiToString(api).c_str());
	} else {
		log_info("Failed to initialize renderer with %s render driver, using %s render driver instead.", R_RendererApiToString(api).c_str(), rendererInfo.name);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return 0;
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

