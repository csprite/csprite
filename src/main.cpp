// For Converting Strings To LowerCase in FixFileExtension function
#include <algorithm>
#include <cctype>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "log/log.h"
#include "tinyfiledialogs.h"

#include "main.h"
#include "save.h"
#include "macros.h"
#include "assets.h"
#include "palette.h"
#include "settings.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

std::string FilePath = "untitled.png"; // Default Output Filename
char const* FileFilterPatterns[3] = { "*.png", "*.jpg", "*.jpeg" };
unsigned int NumOfFilterPatterns = 3;

SDL_Window* window = NULL;

int WindowDims[2] = { 700, 500 };
int CanvasDims[2] = { 60, 40 };
int ZoomLevel = 8;
int BrushSize = 5;
std::string ZoomText = "Zoom: " + std::to_string(ZoomLevel) + "x";

unsigned int LastPaletteIndex = 0;
unsigned int PaletteIndex = 0;
palette_t* P = NULL;

#define SelectedColor P->entries[PaletteIndex]

Uint32* CanvasData = NULL;
Uint32* CanvasBgData = NULL;
#define CANVAS_SIZE_B CanvasDims[0] * CanvasDims[1] * sizeof(Uint32)
SDL_Rect CanvasContRect = {}; // Rectangle In Which Our Canvas Will Be Placed

bool IsCtrlDown = false;
bool IsShiftDown = false;
bool IsLMBDown = false;
bool AppCloseRequested = false;
bool ShowNewCanvasWindow = false;
bool ShowSettingsWindow = false;
bool CanvasFreeze = false;
bool ImgDidChange = false;

enum tool_e { BRUSH, ERASER, PAN, FILL, INK_DROPPER, LINE, RECTANGLE };
enum mode_e { SQUARE, CIRCLE };

// Currently & last selected tool
enum tool_e Tool = BRUSH;
enum tool_e LastTool = BRUSH;
enum mode_e Mode = CIRCLE;
enum mode_e LastMode = CIRCLE;

struct cvstate {
	unsigned char* pixels;
	cvstate* next;
	cvstate* prev;
};

struct mousepos {
	double X;
	double Y;
	double LastX;
	double LastY;
	double DownX;
	double DownY;
};

typedef struct cvstate cvstate_t;
typedef struct mousepos mousepos_t;

mousepos_t MousePos = { 0 };
mousepos_t MousePosRel = { 0 };
cvstate_t* CurrentState = NULL;
settings_t* AppSettings = NULL;

// #define MAX_DEF_SDL_RENDERER_SIZE 128
// #if defined(__APPLE__)
// 	char DefaultSdlRenderer[MAX_DEF_SDL_RENDERER_SIZE] = "metal";
// #elif defined(__linux__) || defined(__unix__)
// 	char DefaultSdlRenderer[MAX_DEF_SDL_RENDERER_SIZE] = "opengl";
// #elif defined(_WIN32) || defined(WIN32)
// 	char DefaultSdlRenderer[MAX_DEF_SDL_RENDERER_SIZE] = "direct3d";
// #else
// 	char DefaultSdlRenderer[MAX_DEF_SDL_RENDERER_SIZE] = "Software";
// #endif

#define UpdateCanvasRect()                                                  \
	CanvasContRect = {                                                      \
		.x = (int)(WindowDims[0] / 2) - (CanvasDims[0] * ZoomLevel / 2),    \
		.y = (int)(WindowDims[1] / 2) - (CanvasDims[1] * ZoomLevel / 2),    \
		.w = (int)CanvasDims[0] * ZoomLevel,                                \
		.h = (int)CanvasDims[1] * ZoomLevel                                 \
	}                                                                       \

static double get_scale(void) {
#ifndef __APPLE__
	float dpi;
	if (SDL_GetDisplayDPI(0, NULL, &dpi, NULL) == 0)
		return dpi / 96.0;
#endif
	return 1.0;
}

int main(int argc, char** argv) {
	AppSettings = LoadSettings();
	if (AppSettings == NULL) {
		log_error("failed to load settings!");
		return -1;
	} else {
		log_info(
			"settings loaded successfully!\n - vsync: %s\n - renderer: %s\n - hardware acceleration: %s",
			AppSettings->vsync == true ? "enabled" : "disabled",
			AppSettings->renderer,
			AppSettings->accelerated == true ? "enabled" : "disabled"
		);
	}

	Uint32 sdl_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	Uint32 sdl_renderer_flags = 0;

	if (AppSettings->vsync)
		sdl_renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
	if (AppSettings->accelerated)
		sdl_renderer_flags |= SDL_RENDERER_ACCELERATED;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		log_error("failed to initialize SDL2: %s", SDL_GetError());
		return -1;
	}

#ifdef _WIN32
	#include <winuser.h>
	SetProcessDPIAware();
#endif

	window = SDL_CreateWindow(WINDOW_TITLE_CSTR, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowDims[0], WindowDims[1], sdl_window_flags);
	SDL_EnableScreenSaver();

#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR /* Available since 2.0.8 */
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif

#if SDL_VERSION_ATLEAST(2, 0, 8)
	/* This hint tells SDL to respect borderless window as a normal window.
	** For example, the window will sit right on top of the taskbar instead
	** of obscuring it. */
	SDL_SetHint("SDL_BORDERLESS_WINDOWED_STYLE", "1");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 12)
	/* This hint tells SDL to allow the user to resize a borderless windoow.
	** It also enables aero-snap on Windows apparently. */
	SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "1");
#endif


#ifdef ENABLE_WIN_ICON
	{
		unsigned char* winIcon = (unsigned char*)assets_get("data/icons/icon-48.png", NULL);
		SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(winIcon, 48, 48, 8, 48 * 2, 0x0f00, 0x00f0, 0x000f, 0xf000);
		SDL_SetWindowIcon(window, surface);
		SDL_FreeSurface(surface);
	}
#endif

	if (SDL_SetHint(SDL_HINT_RENDER_DRIVER, AppSettings->renderer) == SDL_TRUE) {
		log_info("requested to use %s renderer.", AppSettings->renderer);
	} else {
		log_error("request failed to use %s renderer!", AppSettings->renderer);
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, sdl_renderer_flags);
	if (renderer == NULL) {
		SDL_Log("Error creating SDL_Renderer: %s", SDL_GetError());
		return -1;
	}

	SDL_RendererInfo rendererInfo;
	SDL_GetRendererInfo(renderer, &rendererInfo);

	if (strncmp(rendererInfo.name, AppSettings->renderer, 128) == 0) {
		log_info("initialized app with %s renderer!", AppSettings->renderer);
	} else {
		log_info("failed to initialize app with %s renderer! using %s renderer instead.", AppSettings->renderer, rendererInfo.name);
		strncpy(AppSettings->renderer, rendererInfo.name, 128);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;

	ImDrawList* ImGuiDrawList = NULL;
	const void* Montserrat_Bold = NULL;
	int Montserrat_Bold_Size = 0;
	Montserrat_Bold = assets_get("data/fonts/Montserrat-Bold.ttf", &Montserrat_Bold_Size);
	io.Fonts->AddFontFromMemoryCompressedTTF(Montserrat_Bold, Montserrat_Bold_Size, 16.0f);
	ImGui::StyleColorsDark();
	P = LoadCsvPalette((const char*)assets_get("data/palettes/cc-29.csv", NULL));

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	ImVec4 EditorBG = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);

	SDL_Texture* CanvasTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CanvasDims[0], CanvasDims[1]);
	SDL_Texture* CanvasBgTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, CanvasDims[0], CanvasDims[1]);

	if (CanvasData == NULL) {
		CanvasData = (Uint32*)malloc(CANVAS_SIZE_B);
		memset(CanvasData, 0, CANVAS_SIZE_B);
		if (CanvasData == NULL) {
			log_error("failed to allocate %d bytes for canvas.\n", CANVAS_SIZE_B);
			return -1;
		}
	}

	if (CanvasBgData == NULL) {
		CanvasBgData = (Uint32*)malloc(CANVAS_SIZE_B);
		if (CanvasBgData == NULL) {
			log_error("failed to allocate %d bytes for checkerboard bg.\n", CANVAS_SIZE_B);
			return -1;
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

	SaveState();

	while (!AppCloseRequested) {
		ProcessEvents();

		SDL_UpdateTexture(CanvasTex, NULL, CanvasData, CanvasDims[0] * sizeof(Uint32));
		SDL_UpdateTexture(CanvasBgTex, NULL, CanvasBgData, CanvasDims[0] * sizeof(Uint32));

		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		{
			static int NEW_DIMS[2] = {60, 40};

			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("New", "Ctrl+N")) {
						ShowNewCanvasWindow = 1;
						CanvasFreeze = true;
					}
					if (ImGui::MenuItem("Open", "Ctrl+O")) {
						char *filePath = tinyfd_openFileDialog("Open A File", NULL, NumOfFilterPatterns, FileFilterPatterns, "Image File (.png, .jpg, .jpeg)", 0);
						if (filePath != NULL) {
							FilePath = std::string(filePath);

							LoadImageToCanvas(FilePath.c_str(), CanvasDims, &CanvasData);
							SDL_SetWindowTitle(window, WINDOW_TITLE_CSTR);

							SDL_DestroyTexture(CanvasTex);
							SDL_DestroyTexture(CanvasBgTex);
							CanvasTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CanvasDims[0], CanvasDims[1]);
							CanvasBgTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, CanvasDims[0], CanvasDims[1]);

							SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
							SDL_SetTextureBlendMode(CanvasTex, SDL_BLENDMODE_BLEND);
							SDL_SetTextureBlendMode(CanvasBgTex, SDL_BLENDMODE_BLEND);

							UpdateCanvasRect();
						}
					}
					if (ImGui::BeginMenu("Save")) {
						if (ImGui::MenuItem("Save", "Ctrl+S")) {
							FilePath = FixFileExtension(FilePath);
							SaveImageFromCanvas(FilePath);
							SDL_SetWindowTitle(window, WINDOW_TITLE_CSTR);
							FreeHistory();
							SaveState();
						}
						if (ImGui::MenuItem("Save As", "Alt+S")) {
							char *filePath = tinyfd_saveFileDialog("Save A File", NULL, NumOfFilterPatterns, FileFilterPatterns, "Image File (.png, .jpg, .jpeg)");
							if (filePath != NULL) {
								FilePath = FixFileExtension(std::string(filePath));
								SaveImageFromCanvas(FilePath);
								SDL_SetWindowTitle(window, WINDOW_TITLE_CSTR);
								FreeHistory();
								SaveState();
							}
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit")) {
					if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
						Undo();
					}
					if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
						Redo();
					}
					if (ImGui::MenuItem("Preferences")) {
						ShowSettingsWindow = true;
						CanvasFreeze = true;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Help")) {
					if (ImGui::MenuItem("About")) {
						OpenURL("https://github.com/pegvin/CSprite/wiki/About-CSprite");
					}
					if (ImGui::MenuItem("GitHub")) {
						OpenURL("https://github.com/pegvin/CSprite");
					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			if (ShowSettingsWindow) {
				ImGui::SetNextWindowSize({240.0f, 140.0f}, 0);
				if (ImGui::BeginPopupModal(
						"ShowSettingsWindow",
						NULL,
						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoResize   |
						ImGuiWindowFlags_NoMove
				)) {
					static bool vsync = AppSettings->vsync;
					static bool accel = AppSettings->accelerated;
					static int currItemIdx = 0;
					static const char* rendererList[5] = { "OpenGL", "Vulkan", "Metal", "Direct3D", "Software" };
					ImGui::Checkbox("VSync", &vsync);
					ImGui::Checkbox("Hardware accelerated", &accel);
					ImGui::Combo("Renderer", &currItemIdx, rendererList, 5, -1);

					if (ImGui::Button("Ok")) {
						AppSettings->vsync = vsync;
						AppSettings->accelerated = accel;
						strncpy(AppSettings->renderer, rendererList[currItemIdx], 128);
						WriteSettings(AppSettings);
						CanvasFreeze = false;
						ShowSettingsWindow = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel")) {
						CanvasFreeze = false;
						ShowSettingsWindow = false;
					}
					ImGui::EndPopup();
				} else {
					ImGui::OpenPopup("ShowSettingsWindow");
				}
			}

			if (ShowNewCanvasWindow) {
				ImGui::SetNextWindowSize({230.0f, 100.0f}, 0);
				if (ImGui::BeginPopupModal(
						"ShowNewCanvasWindow",
						NULL,
						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoTitleBar |
						ImGuiWindowFlags_NoResize   |
						ImGuiWindowFlags_NoMove
				)) {
					ImGui::InputInt("width", &NEW_DIMS[0], 1, 1, 0);
					ImGui::InputInt("height", &NEW_DIMS[1], 1, 1, 0);

					if (ImGui::Button("Ok")) {
						FreeHistory();
						free(CanvasData);
						free(CanvasBgData);
						CanvasDims[0] = NEW_DIMS[0];
						CanvasDims[1] = NEW_DIMS[1];

						CanvasData = (Uint32*)malloc(CANVAS_SIZE_B);
						memset(CanvasData, 0, CANVAS_SIZE_B);

						if (CanvasData == NULL) {
							printf("Unable To allocate memory for canvas.\n");
							return 1;
						}

						CanvasBgData = (Uint32*)malloc(CANVAS_SIZE_B);
						if (CanvasBgData == NULL) {
							printf("Unable To allocate memory for canvas.\n");
							return 1;
						}

						for (int x = 0; x < CanvasDims[0]; x++) {
							for (int y = 0; y < CanvasDims[1]; y++) {
								Uint32* pixel = GetPixel(x, y, CanvasBgData);
								*pixel = (x + y) % 2 ? 0x000000FF : 0xFFFFFFFF;
							}
						}

						SDL_DestroyTexture(CanvasTex);
						SDL_DestroyTexture(CanvasBgTex);
						CanvasTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CanvasDims[0], CanvasDims[1]);
						CanvasBgTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, CanvasDims[0], CanvasDims[1]);

						SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
						SDL_SetTextureBlendMode(CanvasTex, SDL_BLENDMODE_BLEND);
						SDL_SetTextureBlendMode(CanvasBgTex, SDL_BLENDMODE_BLEND);

						UpdateCanvasRect();
						SaveState();
						CanvasFreeze = false;
						ShowNewCanvasWindow = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel")) {
						CanvasFreeze = false;
						ShowNewCanvasWindow = false;
					}
					ImGui::EndPopup();
				} else {
					ImGui::OpenPopup("ShowNewCanvasWindow");
				}
			}

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

			if (ImGui::Begin("PWindow", NULL, window_flags)) {
				ImGui::SetWindowSize({70.0f, (float)WindowDims[1]});
				ImGui::SetWindowPos({0.0f, 25.0f});
				for (unsigned int i = 0; i < P->numOfEntries; i++) {
					ImGuiDrawList = ImGui::GetWindowDrawList();
					if (i != 0 && i % 2 != 0)
						ImGui::SameLine();

					if (ImGui::ColorButton(PaletteIndex == i ? "Selected Color" : ("Color##" + std::to_string(i)).c_str(), {(float)((P->entries[i] >> 24) & 0xFF)/255, (float)((P->entries[i] >> 16) & 0xFF)/255, (float)((P->entries[i] >> 8) & 0xFF)/255, (float)(P->entries[i] & 0xFF)/255}))
						PaletteIndex = i;

					if (PaletteIndex == i)
						ImGuiDrawList->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 0xFFFFFFFF, 0, 0, 1);
				};
				ImGui::End();
			}

		}

		ImGui::Render();
		SDL_SetRenderDrawColor(renderer, (Uint8)(EditorBG.x * 255), (Uint8)(EditorBG.y * 255), (Uint8)(EditorBG.z * 255), (Uint8)(EditorBG.w * 255));

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

	FreeHistory();
	FreePalette(P);
	free(CanvasData);
	free(CanvasBgData);
	free(AppSettings);

	// Unneccessary but why not?
	P = NULL;
	CanvasTex = NULL;
	CanvasBgTex = NULL;
	renderer = NULL;
	window = NULL;
	CanvasData = NULL;
	CanvasBgData = NULL;
	AppSettings = NULL;
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
			} else if (event.key.keysym.sym == SDLK_b && !CanvasFreeze) {
				Tool = BRUSH;
				Mode = IsShiftDown == true ? SQUARE : CIRCLE;
			} else if (event.key.keysym.sym == SDLK_e && !CanvasFreeze) {
				Tool = ERASER;
				Mode = IsShiftDown == true ? SQUARE : CIRCLE;
			} else if (event.key.keysym.sym == SDLK_f && !CanvasFreeze) {
				Tool = FILL;
			} else if (event.key.keysym.sym == SDLK_l && !CanvasFreeze) {
				Tool = LINE;
				Mode = IsShiftDown == true ? SQUARE : CIRCLE;
			} else if (event.key.keysym.sym == SDLK_r && !CanvasFreeze) {
				Tool = RECTANGLE;
				Mode = IsShiftDown == true ? SQUARE : CIRCLE;
			} else if (event.key.keysym.sym == SDLK_EQUALS && !CanvasFreeze) {
				if (IsCtrlDown == true) {
					AdjustZoom(true);
				} else {
					BrushSize++;
				}
			} else if (event.key.keysym.sym == SDLK_MINUS && !CanvasFreeze) {
				if (IsCtrlDown == true) {
					AdjustZoom(false);
				} else {
					BrushSize--;
					BrushSize = BrushSize < 1 ? 1 : BrushSize; // Clamp So It Doesn't Go Below 1
				}
			} else if (event.key.keysym.sym == SDLK_SPACE && !CanvasFreeze) {
				if (Tool != PAN) {
					LastTool = Tool;
					Tool = PAN;
				}
			} else if (event.key.keysym.sym == SDLK_i && !CanvasFreeze) {
				if (Tool != INK_DROPPER) {
					LastTool = Tool;
					Tool = INK_DROPPER;
				}
			} else if (event.key.keysym.sym == SDLK_LEFTBRACKET && !CanvasFreeze) {
				if (PaletteIndex != 0) {
					LastPaletteIndex = PaletteIndex;
					PaletteIndex--;
				} else {
					LastPaletteIndex = PaletteIndex;
					PaletteIndex = P->numOfEntries - 1;
				}
			} else if (event.key.keysym.sym == SDLK_RIGHTBRACKET && !CanvasFreeze) {
				if (PaletteIndex < P->numOfEntries - 1) {
					LastPaletteIndex = PaletteIndex;
					PaletteIndex++;
				} else {
					LastPaletteIndex = PaletteIndex;
					PaletteIndex = 0;
				}
			}
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
				IsShiftDown = false;
			else if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL)
				IsCtrlDown = false;
			else if (event.key.keysym.sym == SDLK_SPACE && !CanvasFreeze)
				Tool = LastTool;
			else if (event.key.keysym.sym == SDLK_z && IsCtrlDown && !CanvasFreeze)
				Undo();
			else if (event.key.keysym.sym == SDLK_y && IsCtrlDown && !CanvasFreeze)
				Redo();
			break;
		case SDL_MOUSEWHEEL:
			if (event.wheel.y > 0 && IsCtrlDown) { // Scroll Up - Zoom In
				AdjustZoom(true);
			} else if (event.wheel.y < 0 && IsCtrlDown) { // Scroll Down - Zoom Out
				AdjustZoom(false);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) {
				IsLMBDown = false;
				ImgDidChange = ImgDidChange || (Tool == LINE || Tool == RECTANGLE);
				if (ImgDidChange == true) {
					SaveState();
					ImgDidChange = false;
				}
				if (Tool == INK_DROPPER) {
					Uint32* color = GetPixel(MousePosRel.X, MousePosRel.Y);
					if (color != NULL) {
						for (unsigned int i = 0; i < P->numOfEntries; i++) {
							if (P->entries[i] == *color) {
								LastPaletteIndex = PaletteIndex;
								PaletteIndex = i;
								Tool = LastTool;
								break;
							}
						}
					}
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT && !CanvasFreeze) {
				IsLMBDown = true;
				MousePosRel.DownX = MousePosRel.X;
				MousePosRel.DownY = MousePosRel.Y;

				if (
					MousePosRel.X >= 0            &&
					MousePosRel.X < CanvasDims[0] &&
					MousePosRel.Y >= 0            &&
					MousePosRel.Y < CanvasDims[1]
				) {
					if (Tool == BRUSH || Tool == ERASER) {
						draw(MousePosRel.X, MousePosRel.Y);
						ImgDidChange = true;
					} else if (Tool == FILL) {
						Uint32* oldColor = GetPixel(MousePosRel.X, MousePosRel.Y);
						if (oldColor != NULL)
							fill(MousePosRel.X, MousePosRel.Y, *oldColor);
					}
				}
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

			if (Tool == PAN && !CanvasFreeze) {
				CanvasContRect.x = CanvasContRect.x + (MousePos.X - MousePos.LastX);
				CanvasContRect.y = CanvasContRect.y + (MousePos.Y - MousePos.LastY);
			} else if (IsLMBDown == true && !CanvasFreeze) {
				if (MousePosRel.X >= 0 && MousePosRel.X < CanvasDims[0] && MousePosRel.Y >= 0 && MousePosRel.Y < CanvasDims[1]) {
					if (Tool == BRUSH || Tool == ERASER) {
						draw(MousePosRel.X, MousePosRel.Y);
						drawInBetween(MousePosRel.X, MousePosRel.Y, MousePosRel.LastX, MousePosRel.LastY);
					}
				}
			}
			break;
		}
	}

	if (
		MousePosRel.X >= 0 && MousePosRel.X < CanvasDims[0] &&
		MousePosRel.Y >= 0 && MousePosRel.Y < CanvasDims[1] &&
		IsLMBDown == true                                   &&
		(Tool == LINE || Tool == RECTANGLE)
	) {
		if (CurrentState->prev != NULL) {
			memcpy(CanvasData, CurrentState->pixels, CANVAS_SIZE_B);
		} else {
			memset(CanvasData, 0, CANVAS_SIZE_B);
		}

		if (Tool == LINE) {
			drawLine(MousePosRel.DownX, MousePosRel.DownY, MousePosRel.X, MousePosRel.Y);
		} else {
			drawRect(MousePosRel.DownX, MousePosRel.DownY, MousePosRel.X, MousePosRel.Y);
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
		if (ZoomLevel < INT_MAX) { // Max Value Of Unsigned int
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
					*ptr = Tool == ERASER ? 0x00000000 : SelectedColor;
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
				*ptr = Tool == ERASER ? 0x00000000 : SelectedColor;
		}
	}
}


// Fill Tool, Fills The Whole Canvas Using Recursion
void fill(int x, int y, Uint32 old_color) {
	if (!(x >= 0 && x < CanvasDims[0] && y >= 0 && y < CanvasDims[1]))
		return;

	Uint32* ptr = GetPixel(x, y);
	if (ptr != NULL && *ptr == old_color) {
		ImgDidChange = true;
		*ptr = SelectedColor;

		fill(x + 1, y, old_color);
		fill(x - 1, y, old_color);
		fill(x, y + 1, old_color);
		fill(x, y - 1, old_color);
	}
}

// Bresenham's line algorithm
void drawLine(int x0, int y0, int x1, int y1) {
	int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */

	for (;;) {
		draw(x0, y0);
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}

/*
 In Simplest form a rectangle is made up of 4 lines,
 this is how we make our rectangle using 2 x, y co-ords.

 Since we're using the drawLine Function for making our,
 rectangle we don't need to worry about round edges.

 x0, y0           x1, y0
   .------->--------.
   |                |
   |                |
   ^                v
   |                |
   |                |
   .-------<--------.
 x0, y1           x1, y1

 XX - Could be converted to a macro?
*/

void drawRect(int x0, int y0, int x1, int y1) {
	drawLine(x0, y0, x1, y0);
	drawLine(x1, y0, x1, y1);
	drawLine(x1, y1, x0, y1);
	drawLine(x0, y1, x0, y0);
}

// Makes sure that the file extension is .png or .jpg/.jpeg
std::string FixFileExtension(std::string filepath) {
	std::string fileExt = filepath.substr(filepath.find_last_of(".") + 1);
	std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), [](unsigned char c){ return std::tolower(c); });

	if (fileExt != "png" && fileExt != "jpg" && fileExt != "jpeg") {
		filepath = filepath + ".png";
	}

	return filepath;
}

void SaveImageFromCanvas(std::string filepath) {
	std::string fileExt = filepath.substr(filepath.find_last_of(".") + 1);
	// Convert File Extension to LowerCase
	std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), [](unsigned char c){ return std::tolower(c); });

	if (fileExt == "png") {
		WritePngFromCanvas(filepath.c_str(), CanvasDims, CanvasData);
	} else if (fileExt == "jpg" || fileExt == "jpeg") {
		WriteJpgFromCanvas(filepath.c_str(), CanvasDims, CanvasData);
	} else {
		filepath = filepath + ".png";
		WritePngFromCanvas(filepath.c_str(), CanvasDims, CanvasData);
	}
}

/*
	Pushes Pixels On Current Canvas in "History" array at index "HistoryIndex"
	Removes The Elements in a range from "History" if "IsDirty" is true
*/
void SaveState() {
	// Runs When We Did Undo And Tried To Modify The Canvas
	if (CurrentState != NULL && CurrentState->next != NULL) {
		cvstate_t* tmp;
		cvstate_t* head = CurrentState->next; // we start freeing from the next node of current node

		while (head != NULL) {
			tmp = head;
			head = head->next;
			if (tmp->pixels != NULL) {
				free(tmp->pixels);
			}
			free(tmp);
		}
	}

	cvstate_t* NewState = (cvstate_t*) malloc(sizeof(cvstate_t));
	NewState->pixels = (unsigned char*) malloc(CANVAS_SIZE_B);

	if (CurrentState == NULL) {
		CurrentState = NewState;
		CurrentState->prev = NULL;
		CurrentState->next = NULL;
	} else {
		NewState->prev = CurrentState;
		NewState->next = NULL;
		CurrentState->next = NewState;
		CurrentState = NewState;
	}

	memset(CurrentState->pixels, 0, CANVAS_SIZE_B);
	memcpy(CurrentState->pixels, CanvasData, CANVAS_SIZE_B);
}

// Undo - Puts The Pixels from "History" at "HistoryIndex"
void Undo() {
	if (CurrentState->prev != NULL) {
		CurrentState = CurrentState->prev;
		memcpy(CanvasData, CurrentState->pixels, CANVAS_SIZE_B);
	}
}

// Redo - Puts The Pixels from "History" at "HistoryIndex"
void Redo() {
	if (CurrentState->next != NULL) {
		CurrentState = CurrentState->next;
		memcpy(CanvasData, CurrentState->pixels, CANVAS_SIZE_B);
	}
}

/*
	Function: FreeHistory()
	Takes The CurrentState Node
		- Frees All Of The Nodes Before It
		- Frees All Of The Nodes After It
*/
void FreeHistory() {
	if (CurrentState == NULL) return;

	cvstate_t* tmp;
	cvstate_t* head = CurrentState->prev;

	while (head != NULL) {
		tmp = head;
		head = head->prev;
		if (tmp != NULL && tmp->pixels != NULL) {
			free(tmp->pixels);
			tmp->pixels = NULL;
			free(tmp);
			tmp = NULL;
		}
	}

	head = CurrentState;

	while (head != NULL) {
		tmp = head;
		head = head->next;
		if (tmp != NULL && tmp->pixels != NULL) {
			free(tmp->pixels);
			tmp->pixels = NULL;
			free(tmp);
			tmp = NULL;
		}
	}

	CurrentState = NULL;
}
