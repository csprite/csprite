#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>

#include "main.h"
#include "macros.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

SDL_Window* window = NULL;

int WindowDims[2] = { 700, 500 };
int CanvasDims[2] = { 60, 40 };
int BrushSize = 4;
int ZoomLevel = 8;

Uint32* CanvasData = NULL;
Uint32* CanvasBgData = NULL;
#define CANVAS_SIZE_B CanvasDims[0] * CanvasDims[1] * sizeof(Uint32)
SDL_Rect CanvasContRect = {}; // Rectangle In Which Our Canvas Will Be Placed

bool IsCtrlDown = false;
bool IsShiftDown = false;
bool IsLMBDown = false;
bool AppCloseRequested = false;

enum tool_e { BRUSH, ERASER, PAN, FILL, INK_DROPPER, LINE, RECTANGLE };
enum mode_e { SQUARE, CIRCLE };

// Currently & last selected tool
enum tool_e Tool = BRUSH;
enum tool_e LastTool = BRUSH;
enum mode_e Mode = CIRCLE;
enum mode_e LastMode = CIRCLE;

#define UpdateCanvasRect()                                             \
	CanvasContRect = {                                                 \
		.x = (WindowDims[0] / 2) - (CanvasDims[0] * ZoomLevel / 2),    \
		.y = (WindowDims[1] / 2) - (CanvasDims[1] * ZoomLevel / 2),    \
		.w = CanvasDims[0] * ZoomLevel,                                \
		.h = CanvasDims[1] * ZoomLevel                                 \
	}                                                                  \

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowDims[0], WindowDims[1], window_flags);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		SDL_Log("Error creating SDL_Renderer for ImGui!");
		return -1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	SDL_Texture* CanvasTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CanvasDims[0], CanvasDims[1]);
	SDL_Texture* CanvasBgTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, CanvasDims[0], CanvasDims[1]);

	if (CanvasData == NULL) {
		CanvasData = (Uint32*)malloc(CANVAS_SIZE_B);
		memset(CanvasData, 0, CANVAS_SIZE_B);
		if (CanvasData == NULL) {
			printf("Unable To allocate memory for canvas.\n");
			return 1;
		}
	}

	if (CanvasBgData == NULL) {
		CanvasBgData = (Uint32*)malloc(CANVAS_SIZE_B);
		if (CanvasBgData == NULL) {
			printf("Unable To allocate memory for canvas.\n");
			return 1;
		}
	}

	for (int x = 0; x < CanvasDims[0]; x++) {
		for (int y = 0; y < CanvasDims[1]; y++) {
			Uint32* pixel = GetPixel(x, y, CanvasBgData);
			*pixel = (x + y) % 2 ? 0x000000FF : 0xFFFFFFFF;
		}
	}

	UpdateCanvasRect();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(CanvasTex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(CanvasBgTex, SDL_BLENDMODE_BLEND);
	while (!AppCloseRequested) {
		ProcessEvents();

		SDL_UpdateTexture(CanvasTex, NULL, CanvasData, 60 * sizeof(Uint32));
		SDL_UpdateTexture(CanvasBgTex, NULL, CanvasBgData, 60 * sizeof(Uint32));

		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Hello, world!");

			ImGui::SliderInt("Brush Size", &BrushSize, 1, 20);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);

			if (ImGui::Button("Brush"))
				Tool = BRUSH;

			ImGui::SameLine();
			if (ImGui::Button("Eraser"))
				Tool = ERASER;

			if (ImGui::Button("Round"))
				Mode = CIRCLE;

			ImGui::SameLine();

			if (ImGui::Button("Square"))
				Mode = SQUARE;

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::Render();
		SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));

		SDL_RenderClear(renderer); // Render ImGui Stuff To Screen

		/*
			We Render The Textures To The Screen Here.
			Note The Order Of Rendering Matters.
		*/
		SDL_RenderCopy(renderer, CanvasBgTex, NULL, &CanvasContRect);
		SDL_RenderCopy(renderer, CanvasTex, NULL, &CanvasContRect);

		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		// Swap Front & Back Buffers
		SDL_RenderPresent(renderer);
	}

	// Cleanup
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyTexture(CanvasTex);
	SDL_DestroyTexture(CanvasBgTex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	free(CanvasData);
	free(CanvasBgData);

	// Unneccessary but why not?
	CanvasTex = NULL;
	CanvasBgTex = NULL;
	renderer = NULL;
	window = NULL;
	CanvasData = NULL;
	CanvasBgData = NULL;
	return 0;
}

void ProcessEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			AppCloseRequested = true;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				AppCloseRequested = true;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
				IsShiftDown = true;
			else if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL)
				IsCtrlDown = true;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
				IsShiftDown = false;
			else if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL)
				IsCtrlDown = false;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
				IsLMBDown = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
				IsLMBDown = true;
			break;
		case SDL_MOUSEMOTION:
			if (IsLMBDown == true) {
				int x = (event.motion.x - CanvasContRect.x) / ZoomLevel;
				int y = (event.motion.y - CanvasContRect.y) / ZoomLevel;
				if (x >= 0 && x < CanvasDims[0] && y >= 0 && y < CanvasDims[1]) {
					draw(x, y);
				}
			}
			break;
		}
	}
}

Uint32* GetPixel(int x, int y, Uint32* data) {
	if (x >= 0 && x < CanvasDims[0] && y >= 0 && y < CanvasDims[1]) {
		return data != NULL ? &data[(y * CanvasDims[0] + x)] : &CanvasData[(y * CanvasDims[0] + x)];
	}
	return NULL;
}

// Uint32 rgba2Uint32(int r = 255, int g = 255, int b = 255, int a = 255) {
// 	return ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff);
// }

void draw(int st_x, int st_y) {
	// dirY = direction Y
	// dirX = direction X

	// Loops From -BrushSize/2 To BrushSize/2, ex: -6/2 to 6/2 -> -3 to 3
	for (int dirY = -BrushSize / 2; dirY < BrushSize / 2 + 1; dirY++) {
		for (int dirX = -BrushSize / 2; dirX < BrushSize / 2 + 1; dirX++) {
			if (st_x + dirX < 0 || st_x + dirX >= CanvasDims[0] || st_y + dirY < 0 || st_y + dirY > CanvasDims[1])
				continue;

			if (Mode == CIRCLE && dirX * dirX + dirY * dirY > BrushSize / 2 * BrushSize / 2)
				continue;

			Uint32* ptr = GetPixel(st_x + dirX, st_y + dirY, NULL);

			if (ptr != NULL)
				//                                     0xFFFFFFFF                    0x00000000
				*ptr = Tool == BRUSH ? RGBA2UINT32(255, 255, 255, 255) : RGBA2UINT32(0, 0, 0, 0);
		}
	}
}

