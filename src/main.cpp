// For Converting Strings To LowerCase in FixFileExtension function
#include <algorithm>
#include <cctype>

#include "sdl2_wrapper.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "log/log.h"
#include "downloader/downloader.h"
#include "tinyfiledialogs.h"

#include "main.h"
#include "save.h"
#include "macros.h"
#include "assets.h"
#include "palette.h"
#include "settings.h"
#include "vmouse.h"
#include "history.h"
#include "theme.h"
#include "workspace.h"

char const* FileFilterPatterns[3] = { "*.png", "*.jpg", "*.jpeg" };
unsigned int NumOfFilterPatterns = 3;

FILE* LogFilePtr = NULL;
SDL_Window* window = NULL;

#define WORKSPACE_LEN 300
workspace_t* WorkspaceArr[WORKSPACE_LEN] = { NULL };
int CurrentWorkspace = 0;

#define CurrWS WorkspaceArr[CurrentWorkspace]
#define CANVAS_SIZE_B (CurrWS->CanvasDims[0] * CurrWS->CanvasDims[1] * sizeof(Uint32))

int WindowDims[2] = { 700, 500 };
int BrushSize = 5;
std::string ZoomText = "Zoom: 8x";

unsigned int PaletteIndex = 0;
unsigned int ThemeIndex = 0;

palette_arr_t* P_Arr = NULL;
theme_arr_t* T_Arr = NULL;

#define T T_Arr->entries[ThemeIndex]
#define P P_Arr->entries[PaletteIndex]
#define SelectedColor P->entries[CurrWS->ColorIndex]

SDL_Renderer* renderer = NULL;

bool IsCtrlDown = false;
bool IsShiftDown = false;
bool IsLMBDown = false;
bool AppCloseRequested = false;
bool ShowNewCanvasWindow = false;
bool ShowSettingsWindow = false;
bool ShowLoSpecPaletteImporter = false;
bool ShowCloseWithoutSaveWindow = false;
bool CanvasFreeze = false;
bool ImgDidChange = false;
bool MouseInBounds = false;
bool DownloaderAvailable = false;

enum tool_e { BRUSH, ERASER, PAN, FILL, INK_DROPPER, LINE, RECTANGLE, CIRCLE_TOOL, RECT_SELECT, SELECTION_MOVE };
enum mode_e { SQUARE, CIRCLE };

// Currently & last selected tool
enum tool_e Tool = BRUSH;
enum tool_e LastTool = BRUSH;
enum mode_e Mode = CIRCLE;
enum mode_e LastMode = CIRCLE;

typedef struct mousepos {
	double X;
	double Y;
	double LastX;
	double LastY;
	double DownX;
	double DownY;
} mousepos_t;

mousepos_t MousePos = { 0 };
mousepos_t MousePosRel = { 0 };
settings_t* AppSettings = NULL;

float AppScale = 1.0f;

ImDrawList* ImGuiDrawList = NULL;
ImFont* BB_Mini_small = NULL;

int GuiErrorOccured = 0;

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

#define UpdateCanvasRect()                                                                                \
	CurrWS->CanvasContRect = {                                                                     \
		.x = (int)(WindowDims[0] / 2) - (CurrWS->CanvasDims[0] * CurrWS->ZoomLevel / 2),    \
		.y = (int)(WindowDims[1] / 2) - (CurrWS->CanvasDims[1] * CurrWS->ZoomLevel / 2),    \
		.w = (int)CurrWS->CanvasDims[0] * CurrWS->ZoomLevel,                                \
		.h = (int)CurrWS->CanvasDims[1] * CurrWS->ZoomLevel                                 \
	}                                                                                                     \

static double GetScale(void) {
	float ddpi, hdpi, vdpi;
	if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi) == 0) {
		return hdpi / 96.0f;
		// return 2.302083; // Test DPI i got from a tester
	} else {
		log_error("error getting DPI: %s", SDL_GetError());
		return 1.0;
		// return 2.302083;
	}
}

static void _FreeNSaveHistory() {
	FreeHistory(&CurrWS->CurrentState); // Free The History Buffer
	SaveHistory(&CurrWS->CurrentState, CANVAS_SIZE_B, CurrWS->CanvasData); // Create New History Buffer
}

#include "_GuiImpl.h"

// Simple Function Checks for available programs
void _CheckDeps() {
	DownloaderAvailable = DownloaderCheckBackends() == 0;
}

void SaveSelectedData() {
	if (CurrWS->SelectionRect.w == 0 || CurrWS->SelectionRect.h == 0) return;

	if (CurrWS->SelectedData != NULL) {
		free(CurrWS->SelectedData);
		CurrWS->SelectedData = NULL;
	}

	int sel_w = CurrWS->SelectionRect.w / CurrWS->ZoomLevel;
	int sel_h = CurrWS->SelectionRect.h / CurrWS->ZoomLevel;
	CurrWS->SelectedData = (Uint32*) malloc(sel_w * sel_h * sizeof(Uint32));

	for (int x = 0; x < sel_w; ++x) {
		for (int y = 0; y < sel_h; ++y) {
			Uint32* pixel = GetPixel(
				x + (CurrWS->SelectionRect.x - CurrWS->CanvasContRect.x) / CurrWS->ZoomLevel,
				y + (CurrWS->SelectionRect.y - CurrWS->CanvasContRect.y) / CurrWS->ZoomLevel
			);
			if (pixel != NULL) CurrWS->SelectedData[(y * sel_w + x)] = *pixel;
		}
	}
}

void Undo() {
	if (CurrWS->SelectionRect.w == 0 && CurrWS->SelectionRect.h == 0) {
		HISTORY_UNDO(CurrWS->CurrentState, CANVAS_SIZE_B, CurrWS->CanvasData);
	}
}

void Redo() {
	if (CurrWS->SelectionRect.w == 0 && CurrWS->SelectionRect.h == 0) {
		HISTORY_REDO(CurrWS->CurrentState, CANVAS_SIZE_B, CurrWS->CanvasData);
	}
}

static inline ImVec4 color_lighten(ImVec4 c, float k) {
	c.x *= k;
	c.y *= k;
	c.z *= k;
	return c;
}

static inline ImVec4 color_darken(ImVec4 c, float k) {
	c.x /= k;
	c.y /= k;
	c.z /= k;
	return c;
}

int main(int argc, char** argv) {
	atexit(FreeEverything);
#ifdef IS_DEBUG
	LogFilePtr = fopen("csprite.log", "w");
	log_add_fp(LogFilePtr, LOG_TRACE);
#endif

	_CheckDeps();
	CurrWS = InitWorkspace(WindowDims);

	{
		T_Arr = ThemeLoadAll();
		if (T_Arr == NULL || T_Arr->numOfEntries <= 0 || T_Arr->entries == NULL) {
			log_error("failed to load app themes!");
			if (T_Arr != NULL) {
				FreeThemeArr(T_Arr);
				T_Arr = NULL;
			}
		}
	}

	AppSettings = LoadSettings();
	if (AppSettings == NULL) {
		log_error("failed to load settings!");
		return -1;
	} else {
		log_info(
			"settings loaded successfully!\n - vsync: %s\n - renderer: %s\n - hardware acceleration: %s\n - custom cursor: %s",
			AppSettings->vsync == true ? "enabled" : "disabled",
			AppSettings->renderer,
			AppSettings->accelerated == true ? "enabled" : "disabled",
			AppSettings->CustomCursor == true ? "enabled" : "disabled"
		);
	}

	Uint32 sdl_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN;
	Uint32 sdl_renderer_flags = 0;

	if (AppSettings->vsync) sdl_renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
	if (AppSettings->accelerated) sdl_renderer_flags |= SDL_RENDERER_ACCELERATED;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		log_error("failed to initialize SDL2: %s", SDL_GetError());
		return -1;
	}

	{
		SDL_version compiled;
		SDL_version linked;

		SDL_VERSION(&compiled);
		SDL_GetVersion(&linked);
		log_info("Compiled With SDL version %u.%u.%u", compiled.major, compiled.minor, compiled.patch);
		log_info("Linked With SDL version %u.%u.%u", linked.major, linked.minor, linked.patch);
	}

	{
		SDL_DisplayMode dm;
		SDL_GetCurrentDisplayMode(0, &dm);
		WindowDims[0] = dm.w * 0.7;
		WindowDims[1] = dm.h * 0.8;

		window = SDL_CreateWindow(
			WINDOW_TITLE_CSTR,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WindowDims[0], WindowDims[1],
			sdl_window_flags
		);
	}

	AppScale = GetScale();

	log_info("detected scale: %f", AppScale);

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	SDL_EnableScreenSaver();
	SDL_AddEventWatch(_EventWatcher, window);

#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR /* Available since 2.0.8 */
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 12)
	// This hint tells SDL to allow the user to resize a borderless windoow, It also enables aero-snap on Windows apparently.
	SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "1");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 8)
	// This hint tells SDL to respect borderless window as a normal window, For example - the window will sit right on top of the taskbar instead of obscuring it.
	SDL_SetHint("SDL_BORDERLESS_WINDOWED_STYLE", "1");
#endif

#ifdef ENABLE_WIN_ICON
	InitWindowIcon();
#endif

	if (SDL_SetHint(SDL_HINT_RENDER_DRIVER, AppSettings->renderer) == SDL_TRUE) {
		log_info("requested to use %s renderer.", AppSettings->renderer);
	} else {
		log_error("request failed to use %s renderer!", AppSettings->renderer);
	}

	renderer = SDL_CreateRenderer(window, -1, sdl_renderer_flags);
	if (renderer == NULL) {
		SDL_Log("Error creating SDL_Renderer: %s", SDL_GetError());
		return -1;
	}
	SDL_RenderSetScale(renderer, AppScale, AppScale);

	{
		SDL_RendererInfo rendererInfo;
		SDL_GetRendererInfo(renderer, &rendererInfo);

		if (strncmp(rendererInfo.name, AppSettings->renderer, 128) == 0) {
			log_info("initialized app with %s renderer!", AppSettings->renderer);
		} else {
			log_info("failed to initialize app with %s renderer! using %s renderer instead.", AppSettings->renderer, rendererInfo.name);
			strncpy(AppSettings->renderer, rendererInfo.name, 128);
		}
	}

	if (AppSettings->CustomCursor == true)
		VirtualMouseInit(renderer);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuiStyle& style = ImGui::GetStyle();
	io.IniFilename = NULL;

	const void* defaultUiFont = NULL;
	int defaultUiFontSize = 0;
	defaultUiFont = assets_get("data/fonts/bm-mini.ttf", &defaultUiFontSize);
	io.Fonts->AddFontFromMemoryCompressedTTF(defaultUiFont, defaultUiFontSize, 16.0f);
	BB_Mini_small = io.Fonts->AddFontFromMemoryCompressedTTF(defaultUiFont, defaultUiFontSize, 12.0f);

	ImGui::StyleColorsDark();

	P_Arr = PalletteLoadAll();
	if (P_Arr != NULL) {
		for (unsigned int i = 0; i < P_Arr->numOfEntries; ++i) {
			log_info("Loaded Palette: %s...", P_Arr->entries[i]->name);
		}
	} else {
		log_error("cannot load all palettes, loading a single one!");
		P_Arr = (palette_arr_t*) malloc(sizeof(palette_arr_t));
		P_Arr->entries = (palette_t**) malloc(1 * sizeof(palette_t*));
		P_Arr->entries[0] = LoadCsvPalette((const char*)assets_get("data/palettes/cc-29.csv", NULL));
		P_Arr->numOfEntries = 1;
	}

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	ImVec4 EditorBG = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);

	CurrWS->CanvasTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CurrWS->CanvasDims[0], CurrWS->CanvasDims[1]);
	GenCanvasBuff();
	GenCanvasBgTex();
	UpdateCanvasRect();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(CurrWS->CanvasTex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(CurrWS->CanvasBgTex, SDL_BLENDMODE_BLEND);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;

	SaveHistory(&CurrWS->CurrentState, CANVAS_SIZE_B, CurrWS->CanvasData);
	SDL_ShowWindow(window);

	while (!AppCloseRequested) {
		ProcessEvents();

		if (MouseInBounds == true && AppSettings->CustomCursor == true) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);

			// Mouse is being hovered over a ImGui Element Change Cursor To Default
			if (io.WantCaptureMouse == true) {
				VirtualMouseSet(DEFAULT);
			}

			// This Function should be always called after you have done all the Set Calls.
			VirtualMouseUpdate();
		}

		SDL_UpdateTexture(CurrWS->CanvasTex, NULL, CurrWS->CanvasData, CurrWS->CanvasDims[0] * sizeof(Uint32));

		_GuiSetColors(style);
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		{
			_GuiMenuWindow();
			_GuiTextWindow();
			_GuiPaletteWindow();
			_GuiTabWindow();

			if (ShowSettingsWindow) _GuiSettingsWindow();
			if (ShowNewCanvasWindow) _GuiNewCanvasWindow();
			if (ShowCloseWithoutSaveWindow) _GuiCloseWithoutSave();
			if (ShowLoSpecPaletteImporter) _GuiLoSpecPaletteImporter();
			if (GuiErrorOccured != 0) return GuiErrorOccured; // To Be Checked At Last
		}

		ImGui::Render();
		SDL_SetRenderDrawColor(renderer, (Uint8)(EditorBG.x * 255), (Uint8)(EditorBG.y * 255), (Uint8)(EditorBG.z * 255), (Uint8)(EditorBG.w * 255));

		SDL_RenderClear(renderer); // Render ImGui Stuff To Screen

		/*
			We Render The Textures To The Screen Here.
			Note The Order Of Rendering Matters.
		*/
		SDL_RenderCopy(renderer, CurrWS->CanvasBgTex, NULL, &CurrWS->CanvasContRect);
		SDL_RenderCopy(renderer, CurrWS->CanvasTex, NULL, &CurrWS->CanvasContRect);

		if (CurrWS->SelectionRectNew.w != 0 && CurrWS->SelectionRectNew.h != 0) {
			SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
			SDL_RenderDrawRect(renderer, &CurrWS->SelectionRectNew);
		}

		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		if (MouseInBounds == true && AppSettings->CustomCursor == true) {
			VirtualMouseDraw(renderer);
		}

		// Swap Front & Back Buffers
		SDL_RenderPresent(renderer);
	}

	return 0;
}

static void _GuiSetColors(ImGuiStyle& style) {
	if (T != NULL) {
		style.Colors[ImGuiCol_PopupBg] = _U32TOIV4(T->PopupBG);
		style.Colors[ImGuiCol_WindowBg] = _U32TOIV4(T->WindowBG);

		style.Colors[ImGuiCol_Header] = _U32TOIV4(T->Header); // used for MenuItem etc
		style.Colors[ImGuiCol_HeaderHovered] = _U32TOIV4(T->Header_Hovered); // Used for MenuItem etc

		style.Colors[ImGuiCol_Text] = _U32TOIV4(T->Text);
		style.Colors[ImGuiCol_TextDisabled] = _U32TOIV4(T->Text_Disabled); // Used for disabled text and shortcut key texts in menu

		style.Colors[ImGuiCol_Button] = _U32TOIV4(T->Button);
		style.Colors[ImGuiCol_ButtonHovered] = _U32TOIV4(T->Button_Hovered);
		style.Colors[ImGuiCol_ButtonActive] = _U32TOIV4(T->Button_Active);

		style.Colors[ImGuiCol_FrameBg] = _U32TOIV4(T->FrameBG);
		style.Colors[ImGuiCol_FrameBgHovered] = _U32TOIV4(T->FrameBG_Hovered);

		style.Colors[ImGuiCol_TitleBg] = _U32TOIV4(T->TitlebarBG);
		style.Colors[ImGuiCol_TitleBgActive] = _U32TOIV4(T->TitlebarBG_Active); // Is Shown On Active Titlebars

		style.Colors[ImGuiCol_Border] = _U32TOIV4(T->Border);
		style.Colors[ImGuiCol_MenuBarBg] = _U32TOIV4(T->MenuBarBG);
		style.Colors[ImGuiCol_CheckMark] = _U32TOIV4(T->Checkmark); // Used For Checkmarks in Checkboxes & Etc
		style.Colors[ImGuiCol_ModalWindowDimBg] = _U32TOIV4(T->ModalDimming);
	}
}

static int _EventWatcher(void* data, SDL_Event* event) {
	switch (event->type) {
		case SDL_WINDOWEVENT: {
			switch (event->window.event) {
				case SDL_WINDOWEVENT_RESIZED: {
					SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
					if (win == (SDL_Window*)data) {
						SDL_GetWindowSize(win, &WindowDims[0], &WindowDims[1]);
						UpdateCanvasRect();
					}
					break;
				}
				case SDL_WINDOWEVENT_ENTER:
					MouseInBounds = true;
					break;
				case SDL_WINDOWEVENT_LEAVE:
					MouseInBounds = false;
					break;
			}
			break;
		}
		case SDL_DROPFILE: {
			char* filePath = event->drop.file;
			if (filePath != NULL) {
				log_info("file dropped: %s", filePath);

				int lastWs = CurrentWorkspace;
				for (int i = 0; i < WORKSPACE_LEN; ++i) {
					if (WorkspaceArr[i] == NULL) {
						CurrentWorkspace = i;
						CurrWS = InitWorkspace(WindowDims);
						break;
					}
				}

				if (LoadImageToCanvas(filePath, &CurrWS->CanvasDims, &CurrWS->CanvasData) == 0) {
					if (UpdateTextures() != 0)
						return -1;

					GenCanvasBgTex();
					UpdateCanvasRect();

					if (CurrWS->FilePath != NULL) free(CurrWS->FilePath);
					CurrWS->FilePath = strdup((const char*)filePath);
					SDL_SetWindowTitle(window, WINDOW_TITLE_CSTR);
					SaveHistory(&CurrWS->CurrentState, CANVAS_SIZE_B, CurrWS->CanvasData);
				} else {
					if (CurrWS != NULL) {
						FreeWorkspace(CurrWS);
						CurrWS = NULL;
						CurrentWorkspace = lastWs;
					}
				}
				SDL_free(filePath);
			}

			break;
		}
	}
	return 0;
}

void ProcessEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			if (CurrWS->FileHasChanged == true) {
				ShowCloseWithoutSaveWindow = true;
				CanvasFreeze = true;
			} else {
				AppCloseRequested = true;
			}
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
				if (CurrWS->FileHasChanged == true) {
					ShowCloseWithoutSaveWindow = true;
					CanvasFreeze = true;
				} else {
					AppCloseRequested = true;
				}
			}
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
			} else if (event.key.keysym.sym == SDLK_c && !CanvasFreeze) {
				Tool = CIRCLE_TOOL;
			} else if (event.key.keysym.sym == SDLK_s && !CanvasFreeze) {
				Tool = RECT_SELECT;
			} else if (event.key.keysym.sym == SDLK_g && !CanvasFreeze) {
				Tool = SELECTION_MOVE;
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
				if (CurrWS->ColorIndex != 0) {
					CurrWS->LastColorIndex = CurrWS->ColorIndex;
					CurrWS->ColorIndex--;
				} else {
					CurrWS->LastColorIndex = CurrWS->ColorIndex;
					CurrWS->ColorIndex = P->numOfEntries - 1;
				}
			} else if (event.key.keysym.sym == SDLK_RIGHTBRACKET && !CanvasFreeze) {
				if (CurrWS->ColorIndex < P->numOfEntries - 1) {
					CurrWS->LastColorIndex = CurrWS->ColorIndex;
					CurrWS->ColorIndex++;
				} else {
					CurrWS->LastColorIndex = CurrWS->ColorIndex;
					CurrWS->ColorIndex = 0;
				}
			}
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
				IsShiftDown = false;
			else if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL)
				IsCtrlDown = false;
			else if (event.key.keysym.sym == SDLK_SPACE && !CanvasFreeze) {
				Tool = LastTool;
			} else if (event.key.keysym.sym == SDLK_z && IsCtrlDown && !CanvasFreeze) {
				Undo();
			} else if (event.key.keysym.sym == SDLK_y && IsCtrlDown && !CanvasFreeze) {
				Redo();
			}
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
				ImgDidChange = ImgDidChange || (Tool == LINE || Tool == RECTANGLE || Tool == CIRCLE_TOOL);
				if (ImgDidChange == true) {
					SaveSelectedData();
					SaveHistory(&CurrWS->CurrentState, CANVAS_SIZE_B, CurrWS->CanvasData);
					ImgDidChange = false;
				}
				if (Tool == INK_DROPPER) {
					Uint32* color = GetPixel(MousePosRel.X, MousePosRel.Y);
					if (color != NULL) {
						for (unsigned int i = 0; i < P->numOfEntries; i++) {
							if (P->entries[i] == *color) {
								CurrWS->LastColorIndex = CurrWS->ColorIndex;
								CurrWS->ColorIndex = i;
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
				MousePos.DownX = MousePos.X;
				MousePos.DownY = MousePos.Y;

				MousePosRel.DownX = MousePosRel.X;
				MousePosRel.DownY = MousePosRel.Y;

				if (
					MousePosRel.X >= 0                           &&
					MousePosRel.X < CurrWS->CanvasDims[0] &&
					MousePosRel.Y >= 0                           &&
					MousePosRel.Y < CurrWS->CanvasDims[1]
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
			MousePosRel.X = (event.motion.x - CurrWS->CanvasContRect.x) / CurrWS->ZoomLevel;
			MousePosRel.Y = (event.motion.y - CurrWS->CanvasContRect.y) / CurrWS->ZoomLevel;

			if (Tool == PAN && !CanvasFreeze) {
				CurrWS->CanvasContRect.x = CurrWS->CanvasContRect.x + (MousePos.X - MousePos.LastX);
				CurrWS->CanvasContRect.y = CurrWS->CanvasContRect.y + (MousePos.Y - MousePos.LastY);
			} else if (IsLMBDown == true && !CanvasFreeze) {
				if (MousePosRel.X >= 0 && MousePosRel.X < CurrWS->CanvasDims[0] && MousePosRel.Y >= 0 && MousePosRel.Y < CurrWS->CanvasDims[1]) {
					if (Tool == BRUSH || Tool == ERASER) {
						draw(MousePosRel.X, MousePosRel.Y);
						drawInBetween(MousePosRel.X, MousePosRel.Y, MousePosRel.LastX, MousePosRel.LastY);
					}
				}
			}
			break;
		}
	}

	switch (Tool) {
		case BRUSH:
		case ERASER:
			VirtualMouseSet(CROSSHAIR);
			break;
		case INK_DROPPER:
			VirtualMouseSet(EYEDROPPER);
			break;
		case PAN:
			VirtualMouseSet(CLOSE_HAND);
			break;
		default:
			VirtualMouseSet(DEFAULT);
			break;
	}

	if (
		MousePosRel.X >= 0 && MousePosRel.X < CurrWS->CanvasDims[0] &&
		MousePosRel.Y >= 0 && MousePosRel.Y < CurrWS->CanvasDims[1] &&
		IsLMBDown == true
	) {
		if (Tool == LINE || Tool == RECTANGLE || Tool == CIRCLE_TOOL || Tool == SELECTION_MOVE) {
			if (CurrWS->CurrentState->prev != NULL) {
				memcpy(CurrWS->CanvasData, CurrWS->CurrentState->pixels, CANVAS_SIZE_B);
			} else {
				memset(CurrWS->CanvasData, 0, CANVAS_SIZE_B);
			}

			if (Tool == LINE) {
				drawLine(MousePosRel.DownX, MousePosRel.DownY, MousePosRel.X, MousePosRel.Y);
			} else if (Tool == RECTANGLE) {
				drawRect(MousePosRel.DownX, MousePosRel.DownY, MousePosRel.X, MousePosRel.Y);
			} else if (Tool == CIRCLE_TOOL) {
				drawCircle(
					MousePosRel.DownX,
					MousePosRel.DownY,
					(int)sqrt( // Calculates Distance Between 2 x, y points
						(MousePosRel.X - MousePosRel.DownX) * (MousePosRel.X - MousePosRel.DownX) +
						(MousePosRel.Y - MousePosRel.DownY) * (MousePosRel.Y - MousePosRel.DownY)
					)
				);
			} else if (Tool == SELECTION_MOVE) {
				if (IsLMBDown == true && CurrWS->SelectionRect.w != 0 && CurrWS->SelectionRect.h != 0) {
					SaveSelectedData();

					CurrWS->SelectionRectNew.x = ((MousePosRel.X - (CurrWS->SelectionRectNew.w / CurrWS->ZoomLevel) / 2) * CurrWS->ZoomLevel) + CurrWS->CanvasContRect.x;
					CurrWS->SelectionRectNew.y = ((MousePosRel.Y - (CurrWS->SelectionRectNew.h / CurrWS->ZoomLevel) / 2) * CurrWS->ZoomLevel) + CurrWS->CanvasContRect.y;

					int sel_w = CurrWS->SelectionRectNew.w / CurrWS->ZoomLevel;
					int sel_h = CurrWS->SelectionRectNew.h / CurrWS->ZoomLevel;

					// First Erase Everything From The Canvas At The Selected Area
					for (int y = 0; y < sel_h; ++y) {
						for (int x = 0; x < sel_w; ++x) {
							Uint32* pixel = GetPixel(
								x + (CurrWS->SelectionRect.x - CurrWS->CanvasContRect.x) / CurrWS->ZoomLevel,
								y + (CurrWS->SelectionRect.y - CurrWS->CanvasContRect.y) / CurrWS->ZoomLevel,
								NULL
							);
							if (pixel != NULL) {
								*pixel = 0x00000000;
							}
						}
					}

					// Write The Copied Buffer From Selected Area To Canvas
					for (int y = 0; y < sel_h; ++y) {
						for (int x = 0; x < sel_w; ++x) {
							Uint32* pixel = GetPixel(
								x + (CurrWS->SelectionRectNew.x - CurrWS->CanvasContRect.x) / CurrWS->ZoomLevel,
								y + (CurrWS->SelectionRectNew.y - CurrWS->CanvasContRect.y) / CurrWS->ZoomLevel,
								NULL
							);

							if (pixel != NULL) {
								*pixel = CurrWS->SelectedData[(y * sel_w + x)];
							}
						}
					}
				}
			}
		} else if (Tool == RECT_SELECT) {
			CurrWS->SelectionRectNew.x = (CurrWS->CanvasContRect.x + (MousePosRel.DownX * CurrWS->ZoomLevel));
			CurrWS->SelectionRectNew.y = (CurrWS->CanvasContRect.y + (MousePosRel.DownY * CurrWS->ZoomLevel));

			CurrWS->SelectionRectNew.w = MousePosRel.X - MousePosRel.DownX;
			CurrWS->SelectionRectNew.h = MousePosRel.Y - MousePosRel.DownY;

			// Basically it resets the selection by checking if height & width is 0 else it sets the selection's offset
			if (CurrWS->SelectionRectNew.w == 0 && CurrWS->SelectionRectNew.h == 0) {
				CurrWS->SelectionRectNew.w = 0;
				CurrWS->SelectionRectNew.h = 0;
				CurrWS->SelectionRect.w = 0;
				CurrWS->SelectionRect.h = 0;

				if (CurrWS->SelectedData != NULL) {
					free(CurrWS->SelectedData);
					CurrWS->SelectedData = NULL;
				}
			} else {
				CurrWS->SelectionRectNew.w = (CurrWS->SelectionRectNew.w + (CurrWS->SelectionRectNew.w < 0 ? 0 : 1)) * CurrWS->ZoomLevel;
				CurrWS->SelectionRectNew.h = (CurrWS->SelectionRectNew.h + (CurrWS->SelectionRectNew.h < 0 ? 0 : 1)) * CurrWS->ZoomLevel;
			}
			if (CurrWS->SelectedData != NULL) {
				free(CurrWS->SelectedData);
				CurrWS->SelectedData = NULL;
			}

			CurrWS->SelectionRect.x = CurrWS->SelectionRectNew.x;
			CurrWS->SelectionRect.y = CurrWS->SelectionRectNew.y;
			CurrWS->SelectionRect.h = CurrWS->SelectionRectNew.h;
			CurrWS->SelectionRect.w = CurrWS->SelectionRectNew.w;
		}
	}

	CurrWS->FileHasChanged = CurrWS->CurrentState->prev != NULL; // If we have something in our undo buffer it means file has changed
}

/*
	Function: FreeEverything
	Description:
		Frees all the memory, destroys sdl stuff & closes all the files
		Is used with atexit function as a callback
*/
static void FreeEverything(void) {
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	VirtualMouseFree();
	FreeHistory(&CurrWS->CurrentState);

	for (int i = 0; i < WORKSPACE_LEN; ++i) {
		if (WorkspaceArr[i] != NULL) {
			FreeWorkspace(WorkspaceArr[i]);
			WorkspaceArr[i] = NULL;
		}
	}

	if (P_Arr != NULL) { FreePaletteArr(P_Arr); P_Arr = NULL; }
	if (T_Arr != NULL) { FreeThemeArr(T_Arr); T_Arr = NULL; }
	if (LogFilePtr != NULL) { fclose(LogFilePtr); LogFilePtr = NULL; }
	if (AppSettings != NULL) { free(AppSettings); AppSettings = NULL; }
	if (renderer != NULL) { SDL_DestroyRenderer(renderer); renderer = NULL; }
	if (window != NULL) { SDL_DestroyWindow(window); window = NULL; }

	SDL_Quit();
}

// Sets Window Icon
static void InitWindowIcon(void) {
	unsigned char* winIcon = (unsigned char*)assets_get("data/icons/icon-48.png", NULL);
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		winIcon,
		48, 48, 32, 48 * 4,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000
	);
	if (surface == NULL) {
		log_error("failed to set window icon: %s", SDL_GetError());
		return;
	}
	SDL_SetWindowIcon(window, surface);
	SDL_FreeSurface(surface);
}

/*
	Function: UpdateTextures
	Description: Updates The Textures To Use New* Canvas Dimensions
	Remarks:
		- Automatically Destroys Old Textures If They Exist
		- Returns Non-Zero Value On Error & Also Logs The Error
*/
int UpdateTextures(void) {
	if (CurrWS->CanvasTex != NULL) { SDL_DestroyTexture(CurrWS->CanvasTex); CurrWS->CanvasTex = NULL; }
	if (CurrWS->CanvasBgTex != NULL) { SDL_DestroyTexture(CurrWS->CanvasBgTex); CurrWS->CanvasBgTex = NULL; }

	CurrWS->CanvasTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CurrWS->CanvasDims[0], CurrWS->CanvasDims[1]);

	if (CurrWS->CanvasTex == NULL) {
		log_error("failed to create main canvas texture: %s", SDL_GetError());
		return -1;
	}

	GenCanvasBgTex();

	if (CurrWS->CanvasBgTex == NULL) {
		log_error("failed to create main canvas background texture: %s", SDL_GetError());
		return -1;
	}

	if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) != 0) {
		log_error("failed to set blend mode: %s", SDL_GetError());
	}
	if (SDL_SetTextureBlendMode(CurrWS->CanvasTex, SDL_BLENDMODE_BLEND) != 0) {;
		log_error("failed to set texture blend mode: %s", SDL_GetError());
	}
	if (SDL_SetTextureBlendMode(CurrWS->CanvasBgTex, SDL_BLENDMODE_BLEND) != 0) {;
		log_error("failed to set texture blend mode: %s", SDL_GetError());
	}
	return 0;
}

/*
	Function: GenCanvasBgTex
	Description: Generates a SDL_Texture and generates a checkerboard pattern for it
	Remarks:
		- Destroys The Texture if It Exists
		- Generates Checkerboard Pattern For It
		- Stores The Generated Texture in Global Variable
*/
void GenCanvasBgTex(void) {
	if (CurrWS->CanvasBgTex != NULL) {
		SDL_DestroyTexture(CurrWS->CanvasBgTex);
		CurrWS->CanvasBgTex = NULL;
	}

	CurrWS->CanvasBgTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CurrWS->CanvasDims[0], CurrWS->CanvasDims[1]);
	Uint32* pixels = NULL;
	int pitch = 8;
	SDL_LockTexture(CurrWS->CanvasBgTex, NULL, (void**)&pixels, &pitch);
	for (int x = 0; x < CurrWS->CanvasDims[0]; x++) {
		for (int y = 0; y < CurrWS->CanvasDims[1]; y++) {
			Uint32* pixel = GetPixel(x, y, pixels);
			*pixel = (x + y) % 2 ? 0x808080FF : 0xC0C0C0FF;
		}
	}
	SDL_UnlockTexture(CurrWS->CanvasBgTex);
}

// Just Allocates Memory For CanvasData & frees old memory
void GenCanvasBuff(void) {
	if (CurrWS->CanvasData != NULL) { free(CurrWS->CanvasData); CurrWS->CanvasData = NULL; }
	CurrWS->CanvasData = (Uint32*)malloc(CANVAS_SIZE_B);
	memset(CurrWS->CanvasData, 0, CANVAS_SIZE_B);
}

Uint32* GetPixel(int x, int y, Uint32* data) {
	if (x >= 0 && x < CurrWS->CanvasDims[0] && y >= 0 && y < CurrWS->CanvasDims[1]) {
		return data != NULL ? &data[(y * CurrWS->CanvasDims[0] + x)] : &CurrWS->CanvasData[(y * CurrWS->CanvasDims[0] + x)];
	}
	return NULL;
}

void AdjustZoom(bool increase) {
	if (increase == true) {
		if (CurrWS->ZoomLevel < INT_MAX) { // Max Value Of Unsigned int
			CurrWS->ZoomLevel++;
			ZoomText = "Zoom: " + std::to_string(CurrWS->ZoomLevel) + "x";
		}
	} else {
		if (CurrWS->ZoomLevel != 1) { // if zoom is 1 then don't decrease it further
			CurrWS->ZoomLevel--;
			ZoomText = "Zoom: " + std::to_string(CurrWS->ZoomLevel) + "x";
		}
	}

	UpdateCanvasRect();
}

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
				if (st_x + dirX < 0 || st_x + dirX >= CurrWS->CanvasDims[0] || st_y + dirY < 0 || st_y + dirY > CurrWS->CanvasDims[1])
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
			if (st_x + dirX < 0 || st_x + dirX >= CurrWS->CanvasDims[0] || st_y + dirY < 0 || st_y + dirY > CurrWS->CanvasDims[1])
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
	if (!(x >= 0 && x < CurrWS->CanvasDims[0] && y >= 0 && y < CurrWS->CanvasDims[1]))
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

// Mid Point Circle Drawing Algorithm
void drawCircle(int centreX, int centreY, int radius) {
	const int diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y) {
		// Each of the following renders an octant of the circle
		draw(centreX + x, centreY - y);
		draw(centreX + x, centreY + y);
		draw(centreX - x, centreY - y);
		draw(centreX - x, centreY + y);
		draw(centreX + y, centreY - x);
		draw(centreX + y, centreY + x);
		draw(centreX - y, centreY - x);
		draw(centreX - y, centreY + x);

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}
		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
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
		WritePngFromCanvas(filepath.c_str(), CurrWS->CanvasDims, CurrWS->CanvasData);
	} else if (fileExt == "jpg" || fileExt == "jpeg") {
		WriteJpgFromCanvas(filepath.c_str(), CurrWS->CanvasDims, CurrWS->CanvasData);
	} else {
		filepath = filepath + ".png";
		WritePngFromCanvas(filepath.c_str(), CurrWS->CanvasDims, CurrWS->CanvasData);
	}
}
