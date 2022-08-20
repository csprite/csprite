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
unsigned int ZoomLevel = 8;
std::string ZoomText = "Zoom: " + std::to_string(ZoomLevel) + "x";

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

struct mousepos {
	double X;
	double Y;
	double LastX;
	double LastY;
	double DownX;
	double DownY;
};

typedef struct mousepos mousepos_t;

mousepos_t MousePos = { 0 };
mousepos_t MousePosRel = { 0 };

#define UpdateCanvasRect()                                                  \
	CanvasContRect = {                                                      \
		.x = (int)(WindowDims[0] / 2) - (CanvasDims[0] * ZoomLevel / 2),    \
		.y = (int)(WindowDims[1] / 2) - (CanvasDims[1] * ZoomLevel / 2),    \
		.w = (int)CanvasDims[0] * ZoomLevel,                                \
		.h = (int)CanvasDims[1] * ZoomLevel                                 \
	}                                                                       \

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowDims[0], WindowDims[1], SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

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

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;

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
			// ImGui::Begin("Controls");
			// ImGui::SetWindowPos({0, 0});
			// ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			// ImGui::ColorEdit3("clear color", (float*)&clear_color);
			// ImGui::End();

			if (ImGui::Begin("ToolAndZoomWindow", NULL, window_flags | ImGuiWindowFlags_NoBringToFrontOnFocus |  ImGuiWindowFlags_NoFocusOnAppearing)) {
				ImGui::SetWindowPos({0.0f, (float)(WindowDims[1] - 55)});
				std::string selectedToolText;

				switch (Tool) {
					case BRUSH:
						if (Mode == SQUARE)
							selectedToolText = "Square Brush - (Size: " + std::to_string(BrushSize) + ")";
						else
							selectedToolText = "Circle Brush - (Size: " + std::to_string(BrushSize) + ")";
						break;
					case ERASER:
						if (Mode == SQUARE)
							selectedToolText = "Square Eraser - (Size: " + std::to_string(BrushSize) + ")";
						else
							selectedToolText = "Circle Eraser - (Size: " + std::to_string(BrushSize) + ")";
						break;
					case FILL:
						selectedToolText = "Fill";
						break;
					case INK_DROPPER:
						selectedToolText = "Ink Dropper";
						break;
					case PAN:
						selectedToolText = "Panning";
						break;
					case LINE:
						if (Mode == SQUARE)
							selectedToolText = "Square Line - (Size: " + std::to_string(BrushSize) + ")";
						else
							selectedToolText = "Round Line - (Size: " + std::to_string(BrushSize) + ")";
						break;
					case RECTANGLE:
						if (Mode == SQUARE)
							selectedToolText = "Square Rect - (Size: " + std::to_string(BrushSize) + ")";
						else
							selectedToolText = "Round Rect - (Size: " + std::to_string(BrushSize) + ")";
						break;
				}

				ImVec2 textSize1 = ImGui::CalcTextSize(selectedToolText.c_str(), NULL, false, -2.0f);
				ImVec2 textSize2 = ImGui::CalcTextSize(ZoomText.c_str(), NULL, false, -2.0f);
				ImGui::SetWindowSize({(float)(textSize1.x + textSize2.x), (float)(textSize1.y + textSize2.y) * 2}); // Make Sure Text is visible everytime.

				ImGui::Text("%s", selectedToolText.c_str());
				ImGui::Text("%s", ZoomText.c_str());
				ImGui::End();
			}
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
			if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
				IsShiftDown = true;
			} else if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL) {
				IsCtrlDown = true;
			} else if (event.key.keysym.sym == SDLK_b) {
				Tool = BRUSH;
				Mode = IsShiftDown == true ? SQUARE : CIRCLE;
			} else if (event.key.keysym.sym == SDLK_e) {
				Tool = ERASER;
				Mode = IsShiftDown == true ? SQUARE : CIRCLE;
			} else if (event.key.keysym.sym == SDLK_EQUALS) {
				if (IsCtrlDown == true) {
					AdjustZoom(true);
				} else {
					BrushSize++;
				}
			} else if (event.key.keysym.sym == SDLK_MINUS) {
				if (IsCtrlDown == true) {
					AdjustZoom(false);
				} else {
					BrushSize--;
					BrushSize = BrushSize < 1 ? 1 : BrushSize; // Clamp So It Doesn't Go Below 1
				}
			} else if (event.key.keysym.sym == SDLK_SPACE) {
				if (Tool != PAN) {
					LastTool = Tool;
					Tool = PAN;
				}
			}
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
				IsShiftDown = false;
			else if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL)
				IsCtrlDown = false;
			else if (event.key.keysym.sym == SDLK_SPACE)
				Tool = LastTool;
			break;
		case SDL_MOUSEWHEEL:
			if (event.wheel.y > 0 && IsCtrlDown) { // Scroll Up - Zoom In
				AdjustZoom(true);
			} else if (event.wheel.y < 0 && IsCtrlDown) { // Scroll Down - Zoom Out
				AdjustZoom(false);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
				IsLMBDown = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {
				IsLMBDown = true;
				draw(MousePosRel.X, MousePosRel.Y);
			}
			break;
		case SDL_MOUSEMOTION:
			MousePos.LastX = MousePos.X;
			MousePos.LastY = MousePos.Y;
			MousePos.X = event.motion.x;
			MousePos.Y = event.motion.y;

			MousePosRel.LastX = MousePosRel.X;
			MousePosRel.LastY = MousePosRel.Y;
			MousePosRel.X = (event.motion.x - CanvasContRect.x) / ZoomLevel;
			MousePosRel.Y = (event.motion.y - CanvasContRect.y) / ZoomLevel;

			if (Tool == PAN) {
				CanvasContRect.x = CanvasContRect.x + (MousePos.X - MousePos.LastX);
				CanvasContRect.y = CanvasContRect.y + (MousePos.Y - MousePos.LastY);
			} else if (IsLMBDown == true) {
				if (MousePosRel.X >= 0 && MousePosRel.X < CanvasDims[0] && MousePosRel.Y >= 0 && MousePosRel.Y < CanvasDims[1]) {
					draw(MousePosRel.X, MousePosRel.Y);
					drawInBetween(MousePosRel.X, MousePosRel.Y, MousePosRel.LastX, MousePosRel.LastY);
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

void AdjustZoom(bool increase) {
	if (increase == true) {
		if (ZoomLevel < UINT_MAX) { // Max Value Of Unsigned int
			ZoomLevel++;
			ZoomText = "Zoom: " + std::to_string(ZoomLevel) + "x";
		}
	} else {
		if (ZoomLevel != 1) { // if zoom is 1 then don't decrease it further
			ZoomLevel--;
			ZoomText = "Zoom: " + std::to_string(ZoomLevel) + "x";
		}
	}

	UpdateCanvasRect();
}

// Uint32 rgba2Uint32(int r = 255, int g = 255, int b = 255, int a = 255) {
// 	return ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff);
// }

/*
	Function Takes 4 Argument First 2 Are starting x, y coordinates,
	and second 2 are ending x, y coordinates.
	And using a while loop it draws between the 2 given coordinates,
	hence no gap is left when mouse is being moved very fast
*/
void drawInBetween(int st_x, int st_y, int end_x, int end_y) {
	while (st_x != end_x || st_y != end_y) {
		if (st_x < end_x) {
			st_x++;
		}
		if (st_x > end_x) {
			st_x--;
		}
		if (st_y < end_y) {
			st_y++;
		}
		if (st_y > end_y) {
			st_y--;
		}

		for (int dirY = -BrushSize / 2; dirY < BrushSize / 2 + 1; dirY++) {
			for (int dirX = -BrushSize / 2; dirX < BrushSize / 2 + 1; dirX++) {
				if (st_x + dirX < 0 || st_x + dirX >= CanvasDims[0] || st_y + dirY < 0 || st_y + dirY > CanvasDims[1])
					continue;

				if (Mode == CIRCLE && dirX * dirX + dirY * dirY > BrushSize / 2 * BrushSize / 2)
					continue;

				Uint32* ptr = GetPixel(st_x + dirX, st_y + dirY, NULL);
				if (ptr != NULL)
					// 0x00000000, 0xFFFFFFFF
					*ptr = Tool == ERASER ? RGBA2UINT32(0, 0, 0, 0) : RGBA2UINT32(255, 255, 255, 255);
			}
		}
	}
}

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
				// 0x00000000, 0xFFFFFFFF
				*ptr = Tool == ERASER ? RGBA2UINT32(0, 0, 0, 0) : RGBA2UINT32(255, 255, 255, 255);
		}
	}
}

