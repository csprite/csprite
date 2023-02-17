#if defined(_WIN32) && defined(_MSC_VER)
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include <string>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <SDL2/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_extension.h"
#include "ImGuiFileBrowser.h"

#include "utils.h"
#include "log/log.h"
#include "assets.h"
#include "macros.h"
#include "config.h"
#include "theme.h"
#include "palette.h"
#include "history.h"
#include "system.h"
#include "tools/tools.h"
#include "renderer/canvas.h"
#include "renderer/renderer.h"
#include "ifileio/ifileio.h"

int32_t WindowDims[2] = { 700, 500 };
int32_t CanvasDims[2] = { 64, 64 };

int32_t MousePos[2] = { 0, 0 };
int32_t MousePosDown[2] = { 0, 0 };
int32_t MousePosLast[2] = { 0, 0 };
int32_t MousePosRel[2] = { 0, 0 };
int32_t MousePosDownRel[2] = { 0, 0 };
int32_t MousePosRelLast[2] = { 0, 0 };

bool ShouldClose = false;
bool IsLMBDown = false;
bool IsCtrlDown = false;
bool IsShiftDown = false;
bool CanvasMutable = true; // If Canvas's Data Can Be Changed Or Not
bool CanvasLocked = false; // Same As `CanvasMutable` but with conditions like if any window is being hover or not
bool CanvasDidMutate = false;
bool ShowOpenNewFileWindow = false;
bool ShowSaveAsFileWindow = false;
bool ShowCanvasPreviewWindow = true;

char FilePath[SYS_PATHNAME_MAX] = "untitled.png";
char FileName[SYS_FILENAME_MAX] = "untitled.png";

int32_t SelectedLayerIndex = 0;
CanvasLayerArr_T* CanvasLayers = NULL;
#define CURR_CANVAS_LAYER CanvasLayers->layers[SelectedLayerIndex]
enum tool_e { BRUSH_COLOR, BRUSH_ERASER, SHAPE_LINE, SHAPE_RECT, SHAPE_CIRCLE, TOOL_INKDROPPER, TOOL_FLOODFILL, TOOL_PAN };
enum tool_shape_e { CIRCLE, SQUARE };
enum tool_e Tool = BRUSH_COLOR;
enum tool_e LastTool = BRUSH_COLOR;

float PreviewWindowZoom = 1.0f;
float CurrViewportZoom = 1.0f;
SDL_Rect ViewportLoc = { 0, 0, 0, 0 };

int32_t PaletteIndex = 0;
int32_t ThemeIndex = 0;
uint16_t PaletteColorIndex = 2;
uint8_t EraseColor[4] = { 0, 0, 0, 0 };
uint8_t SelectedColor[4] = { 255, 255, 255, 255 };

#define MAX_SELECTEDTOOLTEXT_SIZE 512
char SelectedToolText[MAX_SELECTEDTOOLTEXT_SIZE] = "";

SDL_Window* window = NULL;
#define WINDOW_TITLE_MAX 512 + SYS_FILENAME_MAX
char WindowTitle[WINDOW_TITLE_MAX] = "";

Config_T* AppConfig = NULL;
PaletteArr_T* PaletteArr = NULL;
theme_arr_t* ThemeArr = NULL;

#ifndef CS_VERSION_MAJOR
	#define CS_VERSION_MAJOR 0
#endif

#ifndef CS_VERSION_MAJOR
	#define CS_VERSION_MINOR 0
#endif

#ifndef CS_VERSION_MAJOR
	#define CS_VERSION_PATCH 0
#endif

#ifndef CS_BUILD_STABLE
	#define CS_BUILD_STABLE 0
#endif

#if CS_BUILD_STABLE == 0
	#define CS_BUILD_TYPE "dev"
#else
	#define CS_BUILD_TYPE "stable"
#endif

#define __STR_IMPL_(s) #s     // Stringify Argument
#define STR(s) __STR_IMPL_(s) // Expand Argument

// two macros for assembling the structure of window title on compile time and appending the filename when needed.
#define VERSION_STR "v" STR(CS_VERSION_MAJOR) "." STR(CS_VERSION_MINOR) "." STR(CS_VERSION_PATCH) "-" CS_BUILD_TYPE
#define GEN_WIN_TITLE() do {\
		if (FileName[0] != 0) { snprintf(WindowTitle, WINDOW_TITLE_MAX, "%s - csprite " VERSION_STR, FileName); }\
		else { snprintf(WindowTitle, WINDOW_TITLE_MAX, "csprite " VERSION_STR); }\
	} while(0)

static int  OpenNewFile(const char* fileName);
static void InitWindowIcon();
static void _GuiSetColors(ImGuiStyle& style);
static void _GuiSetToolText();
static void UpdateViewportSize();
static void UpdateViewportPos();
static void ZoomViewport(int increase);
static void MutateCanvas(bool LmbJustReleased);
static inline bool CanMutateCanvas();
static inline void ProcessEvents();
static uint8_t* GetPixel(int x, int y);

#define GetSelectedPalette() PaletteArr->Palettes[PaletteIndex]

#define UNDO() \
	if (CURR_CANVAS_LAYER != NULL) HISTORY_UNDO(CURR_CANVAS_LAYER->history, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t), CURR_CANVAS_LAYER->pixels)

#define REDO() \
	if (CURR_CANVAS_LAYER != NULL) HISTORY_REDO(CURR_CANVAS_LAYER->history, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t), CURR_CANVAS_LAYER->pixels)

#define UPDATE_WINDOW_TITLE() do {\
		GEN_WIN_TITLE();\
		SDL_SetWindowTitle(window, WindowTitle);\
	} while(0)

int main(int argc, char* argv[]) {
	FILE* LogFilePtr = fopen(Sys_GetLogFileName(), "w");
	log_add_fp(LogFilePtr, LOG_TRACE);

	AppConfig = LoadConfig();
	PaletteArr = PaletteLoadAll();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
		log_error("failed to initialize SDL2: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);

	log_info("Init csprite " VERSION_STR);
	log_info("Compiled With SDL version %u.%u.%u", compiled.major, compiled.minor, compiled.patch);
	log_info("Linked With SDL version %u.%u.%u", linked.major, linked.minor, linked.patch);

	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	WindowDims[0] = dm.w * 0.7;
	WindowDims[1] = dm.h * 0.8;

	GEN_WIN_TITLE();
	window = SDL_CreateWindow(
		WindowTitle,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WindowDims[0], WindowDims[1],
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_MAXIMIZED
	);
	UpdateViewportPos();

	InitWindowIcon();
	SDL_ShowWindow(window);
	_GuiSetToolText();

	if (R_Init(window, AppConfig->vsync) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	SDL_Renderer* renderer = R_GetRenderer();

	UpdateViewportPos();
	UpdateViewportSize();

	if (Canvas_Init(CanvasDims[0], CanvasDims[1], renderer) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	CanvasLayers = Canvas_CreateArr(100);
	if (CanvasLayers == NULL) return EXIT_FAILURE;

	CURR_CANVAS_LAYER = Canvas_CreateLayer(renderer);
	if (CURR_CANVAS_LAYER == NULL) return EXIT_FAILURE;
	CanvasLayers->size++;

	if (argc > 1) {
		const char* filePath = (const char*)argv[1];
		int result = Sys_IsRegularFile(filePath);
		if (result < 0) {
			log_error("Error Trying To Valid File Path: %s", strerror(errno));
		} else if (result == 0) {
			log_error("Cannot Open The File in filePath");
		} else {
			OpenNewFile(filePath);
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	ImGui::StyleColorsDark();
	ThemeArr = ThemeLoadAll();
	_GuiSetColors(style);

	{
		int defaultUiFontSize = 0;
		const void* defaultUiFont = Assets_Get("data/fonts/bm-mini.ttf", &defaultUiFontSize);
		if (defaultUiFont) io.Fonts->AddFontFromMemoryCompressedTTF(defaultUiFont, defaultUiFontSize, 16.0f);
	}

	SelectedColor[0] = GetSelectedPalette()->Colors[PaletteColorIndex][0];
	SelectedColor[1] = GetSelectedPalette()->Colors[PaletteColorIndex][1];
	SelectedColor[2] = GetSelectedPalette()->Colors[PaletteColorIndex][2];
	SelectedColor[3] = GetSelectedPalette()->Colors[PaletteColorIndex][3];

	bool ShowPreferencesWindow = false;
	bool ShowLayerRenameWindow = false;
	bool ShowNewCanvasWindow = false;

	unsigned int frameStart, frameTime;
	unsigned int frameDelay = 1000 / AppConfig->FramesUpdateRate;

	imgui_addons::ImGuiFileBrowser ImFileDialog;
	while (!ShouldClose) {
		ProcessEvents();

		frameStart = SDL_GetTicks();
		CanvasLocked = !CanvasMutable || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ShowLayerRenameWindow || ShowPreferencesWindow || ShowNewCanvasWindow;

		R_Clear(); // Clear The Screen, This Is Required To Done Before The Canvas Is Drawn Because Rendered Canvas Is Directly Copied Onto Screen & Clearing The screen After Copying It Will Not Show The Canvas
		R_NewFrame(); // All The Calls To ImGui Will Be Recorded After This Function

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New")) {
					ShowNewCanvasWindow = true;
				}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					ShowOpenNewFileWindow = true;
				}
				if (ImGui::BeginMenu("Save")) {
					if (ImGui::MenuItem("Save", "Ctrl+S")) {
						ifio_write(FilePath, CanvasDims[0], CanvasDims[1], CanvasLayers);
					}
					if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) {
						ShowSaveAsFileWindow = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Undo", "Ctrl+Z", false, (CURR_CANVAS_LAYER != NULL && CURR_CANVAS_LAYER->history->prev != NULL))) {
					UNDO();
				}
				if (ImGui::MenuItem("Redo", "Ctrl+Y", false, (CURR_CANVAS_LAYER != NULL && CURR_CANVAS_LAYER->history->next != NULL))) {
					REDO();
				}
				if (ImGui::BeginMenu("Palette")) {
					for (int32_t i = 0; i < PaletteArr->numOfEntries; ++i) {
						int32_t _palidx = PaletteIndex;

						if (ImGui::MenuItem(PaletteArr->Palettes[i]->name, NULL)) {
							PaletteIndex = i;
						}
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
							ImGui::SetTooltip("%s", PaletteArr->Palettes[i]->author);
						}
						if (_palidx == i) {
							ImGui::SameLine();
							ImGui::Text("<");
						}
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Theme")) {
					for (int32_t i = 0; i < ThemeArr->numOfEntries; ++i) {
						int32_t _themeidx = ThemeIndex;

						if (ImGui::MenuItem(ThemeArr->entries[i]->name, NULL)) {
							ThemeIndex = i;
							_GuiSetColors(style);
						}
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
							ImGui::SetTooltip("%s", ThemeArr->entries[i]->author);
						}

						if (_themeidx == i) {
							ImGui::SameLine();
							ImGui::Text("<");
						}
					}
					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Preferences")) {
					ShowPreferencesWindow = true;
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				if (ImGui::MenuItem("Preview")) {
					if (ShowCanvasPreviewWindow) ShowCanvasPreviewWindow = false;
					else ShowCanvasPreviewWindow = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("Wiki")) {
					Sys_OpenURL("https://csprite.github.io/wiki/");
				}
				if (ImGui::MenuItem("About")) {
					Sys_OpenURL("https://github.com/pegvin/csprite/wiki/About-CSprite");
				}
				if (ImGui::MenuItem("GitHub")) {
					Sys_OpenURL("https://github.com/pegvin/csprite");
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (ShowOpenNewFileWindow) {
			ImGui::OpenPopup("Select a file##Csprite_OpenNewFileDlg");
			ShowOpenNewFileWindow = false;
		}
		if (ShowSaveAsFileWindow) {
			ImGui::OpenPopup("Save file as##Csprite_SaveAsFileDlg");
			ShowSaveAsFileWindow = false;
		}

		if (ImFileDialog.showFileDialog("Select a file##Csprite_OpenNewFileDlg", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(0, 0), ".csprite,.png,.jpg,.jpeg,.bmp")) {
			OpenNewFile(ImFileDialog.selected_path.c_str());
		}
		if (ImFileDialog.showFileDialog("Save file as##Csprite_SaveAsFileDlg", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(0, 0), ".csprite,.png,.jpg,.jpeg,.bmp")) {
			/*  simple logic which appends the appropriate extension if none provided.
				like "test1" with ".png" filter selected is converted to "test1.png" */
			char* _fPath = (char*)ImFileDialog.selected_path.c_str();
			if (!HAS_SUFFIX_CI(_fPath, ImFileDialog.ext.c_str(), ImFileDialog.ext.length())) {
				int newStrLen = ImFileDialog.selected_path.length() + ImFileDialog.ext.length() + 1;
				_fPath = (char*)malloc(newStrLen);
				snprintf(_fPath, newStrLen, "%s%s", ImFileDialog.selected_path.c_str(), ImFileDialog.ext.c_str());
			}
			snprintf(FilePath, SYS_PATHNAME_MAX, "%s", _fPath);
			char* _fName = Sys_GetBasename(_fPath);
			snprintf(FileName, SYS_FILENAME_MAX, "%s", _fName);
			UPDATE_WINDOW_TITLE();
			ifio_write(FilePath, CanvasDims[0], CanvasDims[1], CanvasLayers);
			free(_fName);
			_fName = NULL;
			_fPath = NULL;
		}

		ImVec2 PalWinSize;
		if (ImGui::Begin("Palettes", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
			ImGui::SetWindowPos({ 3.0f, 30.0f });
			for (unsigned int i = 0; i < GetSelectedPalette()->numOfEntries; i++) {
				if (i != 0 && i % 2 != 0) ImGui::SameLine();
				static char ColorButtonId[20] = "";
				if (PaletteColorIndex != i) { snprintf(ColorButtonId, 20, "Color##%d", i); }

				if (ImGui::ColorButton(PaletteColorIndex == i ? "Selected Color" : ColorButtonId, {
					((float)(GetSelectedPalette()->Colors[i][0]) / 255),
					((float)(GetSelectedPalette()->Colors[i][1]) / 255),
					((float)(GetSelectedPalette()->Colors[i][2]) / 255),
					((float)(GetSelectedPalette()->Colors[i][3]) / 255)
				})) {
					PaletteColorIndex = i;
					SelectedColor[0] = GetSelectedPalette()->Colors[PaletteColorIndex][0];
					SelectedColor[1] = GetSelectedPalette()->Colors[PaletteColorIndex][1];
					SelectedColor[2] = GetSelectedPalette()->Colors[PaletteColorIndex][2];
					SelectedColor[3] = GetSelectedPalette()->Colors[PaletteColorIndex][3];
				}

				ImGui::GetWindowDrawList()->AddRect(
					ImGui::GetItemRectMin(),
					ImGui::GetItemRectMax(),
					(PaletteColorIndex == i && COLOR_EQUAL(SelectedColor, GetSelectedPalette()->Colors[i])) ? 0xFFFFFFFF : 0x000000FF,
					0, 0, 1
				);
			};

			ImGui::SetWindowSize({0.0f, 0.0f}); // This Will Make the window adjust size according to children
			PalWinSize = ImGui::GetWindowSize();
			ImGui::End();
		}

		if (ImGui::Begin("###ColorPickerWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground)) {
			ImGui::SetWindowPos({ 0, PalWinSize.y + 40.0f });
			float ImColPicker[4] = { (float)(SelectedColor[0]) / 255, (float)(SelectedColor[1]) / 255, (float)(SelectedColor[2]) / 255, (float)(SelectedColor[3]) / 255 };
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
			ImGui::ColorEdit4(
				"##ColorPickerWidget", (float*)&ImColPicker,
				ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel
			);
			ImGui::PopStyleVar();

			SelectedColor[0] = ImColPicker[0] * 255;
			SelectedColor[1] = ImColPicker[1] * 255;
			SelectedColor[2] = ImColPicker[2] * 255;
			SelectedColor[3] = ImColPicker[3] * 255;

			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 0xFFFFFFFF, 0, 0, 1);
			ImGui::SetWindowSize({ 0, 0 });
			ImGui::End();
		}

		if (ImGui::Begin(
			"Debug Window", NULL,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize
#if(CS_BUILD_STABLE == 1)
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground
#endif
		)) {
			ImVec2 WinSize = ImGui::GetWindowSize();
			ImGui::SetWindowPos({ 5, io.DisplaySize.y - WinSize.y - 10 });
			ImGui::Text("%s\nZoom: %d%%", SelectedToolText, (int)(CurrViewportZoom * 100));
#if(CS_BUILD_STABLE == 0)
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			if (ImGui::Button("Clear Undo/Redo Buffers")) {
				for (int i = 0; i < CanvasLayers->size; ++i) {
					if (CanvasLayers->layers[i] != NULL) {
						FreeHistory(&CanvasLayers->layers[i]->history);
						SaveHistory(&CanvasLayers->layers[i]->history, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t), CanvasLayers->layers[i]->pixels);
					}
				}
			}
#endif
			ImGui::End();
		}

		static int32_t PreviewZoom = 2;
		static ImVec2  PreviewImageSize;
		static bool ResetPreviewWindowPos = true;
		static bool ReCalculateZoomSize = true;

		{
			// simple condition that set ResetPreviewWindowPos to true, so that Preview window position can be updated after the window width is calculated
			static int FramesPassed = 0;
			static bool LockCond = false;
			if (!LockCond) {
				if (FramesPassed != 3) {
					FramesPassed++;
				} else {
					LockCond = true;
					ResetPreviewWindowPos = true;
				}
			}
		}

		if (ShowCanvasPreviewWindow && ImGui::Begin("Preview", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::BeginMenu("Zoom")) {
					if (ImGui::MenuItem("Increase")) { PreviewZoom++; ReCalculateZoomSize = true; }
					if (ImGui::MenuItem("Decrease")) { if (PreviewZoom > 1) { PreviewZoom--; ReCalculateZoomSize = true; } }
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Reset Position")) {
					ResetPreviewWindowPos = true;
				}
				ImGui::EndPopup();
			}

			if (ReCalculateZoomSize) {
				PreviewImageSize.x = CanvasDims[0] * PreviewZoom;
				PreviewImageSize.y = CanvasDims[1] * PreviewZoom;
				ReCalculateZoomSize = false;
			}
			if (ResetPreviewWindowPos == true) {
				ImGui::SetWindowPos({ io.DisplaySize.x - ImGui::GetWindowSize().x - 20, io.DisplaySize.y - ImGui::GetWindowSize().y - 20, }); // Move Window To Bottom Right With 20 pixel padding
				ResetPreviewWindowPos = false;
			}

			ImGui::Image(reinterpret_cast<ImTextureID>(Canvas_GetTex()), PreviewImageSize);
			ImGui::End();
		}

		if (ImGui::Begin("Layer", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			ImVec2 WinSize = { 200, 300 };
			ImGui::SetWindowPos({ io.DisplaySize.x - WinSize.x - 10, 40, }); // Move Window To Bottom Right With 20 pixel padding
			ImGui::SetWindowSize(WinSize, ImGuiCond_FirstUseEver);

			if (ImGui::Button("+")) {
				if (SelectedLayerIndex + 1 >= CanvasLayers->capacity) {
					int32_t newSize = CanvasLayers->capacity + 50;
					Canvas_ResizeArr(CanvasLayers, newSize);
					if (CanvasLayers->capacity == newSize) {
						SelectedLayerIndex++;
						CURR_CANVAS_LAYER = Canvas_CreateLayer(renderer);
						CanvasLayers->size++;
						log_info("Resized canvas layers array to %d", newSize);
					} else {
						log_error("Unable to resize canvas layers array!");
					}
				} else {
					if (CURR_CANVAS_LAYER != NULL) SelectedLayerIndex++; // when opening a .csprite file with no layers CURR_CANVAS_LAYER might be NULL
					CURR_CANVAS_LAYER = Canvas_CreateLayer(renderer);
					CanvasLayers->size++;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("-")) {
				if (CURR_CANVAS_LAYER != NULL) {
					Canvas_DestroyLayer(CURR_CANVAS_LAYER);
					CanvasLayers->size--;
					CURR_CANVAS_LAYER = NULL;
					if (SelectedLayerIndex > 0) SelectedLayerIndex--;
				}
			}

			int move_from = -1, move_to = -1;
			for (int32_t i = 0; i < CanvasLayers->size; ++i) {
				if (CanvasLayers->layers[i] != NULL) {
					if (ImGui::Selectable(CanvasLayers->layers[i]->name, SelectedLayerIndex == i, ImGuiSelectableFlags_AllowDoubleClick)) {
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
							ShowLayerRenameWindow = true;
						} else {
							SelectedLayerIndex = i;
						}
					}

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover | ImGuiDragDropFlags_SourceNoHoldToOpenOthers)) {
						ImGui::Text("Moving \"%s\"", CanvasLayers->layers[i]->name); // tooltip text
						ImGui::SetDragDropPayload("LayersDNDId", &i, sizeof(int));
						ImGui::EndDragDropSource();
					}

					if (ImGui::BeginDragDropTarget()) {
						ImGuiDragDropFlags target_flags = ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LayersDNDId", target_flags)) {
							move_from = *((const int*)payload->Data);
							move_to = i;
						}
						ImGui::EndDragDropTarget();
					}
				}
			}

			if (move_from != -1 && move_to != -1) {
				// Reorder items
				int copy_dst = (move_from < move_to) ? move_from : move_to + 1;
				int copy_src = (move_from < move_to) ? move_from + 1 : move_to;
				CanvasLayer_T* tmp = CanvasLayers->layers[move_from];
				CanvasLayers->layers[copy_dst] = CanvasLayers->layers[copy_src];
				CanvasLayers->layers[move_to] = tmp;
				ImGui::SetDragDropPayload("LayersDNDId", &move_to, sizeof(int));
			}

			ImGui::End();
		}

		if (ShowNewCanvasWindow) {
			if (ImGui::BeginPopupModal("Create New###NewCanvasWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
				static int32_t NewDims[2] = { 64, 64 };
				ImGui::InputInt("Width", &NewDims[0], 1, 5);
				ImGui::InputInt("Height", &NewDims[1], 1, 5);

				if (ImGui::Button("Create")) {
					if (NewDims[0] > 0 && NewDims[1] > 0) {
						Canvas_DestroyArr(CanvasLayers);
						CanvasLayers = Canvas_CreateArr(100);
						SelectedLayerIndex = 0;
						Canvas_Resize(NewDims[0], NewDims[1], R_GetRenderer());
						CURR_CANVAS_LAYER = Canvas_CreateLayer(renderer);
						CanvasLayers->size++;
						CanvasDims[0] = NewDims[0];
						CanvasDims[1] = NewDims[1];
						CurrViewportZoom = 1.0f;
						UpdateViewportSize();
						UpdateViewportPos();
					}
					ShowNewCanvasWindow = false;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					NewDims[0] = 64;
					NewDims[1] = 64;
					ShowNewCanvasWindow = false;
				}
				ImGui::EndPopup();
			} else {
				ImGui::OpenPopup("Create New###NewCanvasWindow");
			}
		}

		if (ShowPreferencesWindow) {
			if (ImGui::BeginPopupModal("Preferences###PreferencesWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("VSync (%s)", AppConfig->vsync ? "Enabled" : "Disabled");
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Vertical Sync - Synchronize App's FPS To Your Monitor's FPS");
				ImGui::SameLine();
				ImGui::Ext_ToggleButton("VSync_Toggle", &AppConfig->vsync);

				ImGui::InputInt("FPS", &AppConfig->FramesUpdateRate, 1, 5, AppConfig->vsync == true ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None);
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Frames Per Second");

				AppConfig->FramesUpdateRate = AppConfig->FramesUpdateRate < 5 ? 5 : AppConfig->FramesUpdateRate;
				if (ImGui::Button("Save")) {
					WriteConfig(AppConfig);
					frameDelay = 1000 / AppConfig->FramesUpdateRate;
					ShowPreferencesWindow = false;
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("please restart the app after saving");
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					ShowPreferencesWindow = false;
				}
				ImGui::EndPopup();
			} else {
				ImGui::OpenPopup("Preferences###PreferencesWindow");
			}
		}

		if (ShowLayerRenameWindow) {
			// This Variable is only true when the popup first appears & is needed to be set to false after the first time of the popup appearing & is needed to be set to true again after the window closes
			static bool isFirstTime = true;
			if (ImGui::BeginPopupModal("Rename Layer###LayerRenameWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
				static char TempBuff[LAYER_NAME_MAX] = "";
				static bool LayerRenamed;
				LayerRenamed = false; // Needed To Be Set To False Every Frame Or Else When Pressing Enter & Pressing A Key It Will Only Read That Single Key Press.

				// if the popup just opened copy the layer name to input buffer
				if (isFirstTime) {
					isFirstTime = false;
					strncpy(TempBuff, CURR_CANVAS_LAYER->name, LAYER_NAME_MAX);
					ImGui::SetKeyboardFocusHere(); // Focus The Next Text Input When Popup Appears
				}
				if (ImGui::InputText("##NewLayerName", TempBuff, LAYER_NAME_MAX, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
					LayerRenamed = true;
				}

				if (ImGui::Button("Ok")) {
					LayerRenamed = true;
				}

				if (LayerRenamed && TempBuff[0] != '\0') {
					strncpy(CURR_CANVAS_LAYER->name, TempBuff, LAYER_NAME_MAX);
					memset(TempBuff, 0, LAYER_NAME_MAX);
					ShowLayerRenameWindow = false;
					LayerRenamed = false;
				}

				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					memset(TempBuff, 0, LAYER_NAME_MAX);
					ShowLayerRenameWindow = false;
				}
				ImGui::EndPopup();
			} else {
				ImGui::OpenPopup("Rename Layer###LayerRenameWindow");
				isFirstTime = true; // set to true cause next frame the popup will appear.
			}
		}

		if (CanvasLayers->size > 0) {
			Canvas_NewFrame(renderer);
			for (int32_t i = 0; i < CanvasLayers->size; ++i) {
				if (CanvasLayers->layers[i] != NULL) {
					Canvas_Layer(CanvasLayers->layers[i], SelectedLayerIndex == i, renderer);
				}
			}
			Canvas_FrameEnd(renderer, &ViewportLoc);

			SDL_SetRenderDrawColor(
				renderer,
				style.Colors[ImGuiCol_Border].x * 255,
				style.Colors[ImGuiCol_Border].y * 255,
				style.Colors[ImGuiCol_Border].z * 255,
				style.Colors[ImGuiCol_Border].w * 255
			);
			SDL_RenderDrawRect(renderer, &ViewportLoc);
		}

		R_Present();

		if (!AppConfig->vsync) {
			frameTime = SDL_GetTicks() - frameStart;
			if (frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
			frameStart = SDL_GetTicks();
		}
	}

	Canvas_DestroyArr(CanvasLayers);
	Canvas_Destroy();
	R_Destroy();
	SDL_DestroyWindow(window);
	SDL_Quit();
	FreePaletteArr(PaletteArr);

	window = NULL;
	PaletteArr = NULL;
	CanvasLayers = NULL;
	return EXIT_SUCCESS;
}

static void _GuiSetToolText() {
	switch (Tool) {
	case BRUSH_COLOR:
	case BRUSH_ERASER:
		snprintf(SelectedToolText, MAX_SELECTEDTOOLTEXT_SIZE, "%s %s (Size: %d)", Tools_GetBrushShape() ? "Square" : "Circle", Tool == BRUSH_COLOR ? "Brush" : "Eraser", Tools_GetBrushSize());
		break;
	case SHAPE_RECT:
	case SHAPE_LINE:
		snprintf(SelectedToolText, MAX_SELECTEDTOOLTEXT_SIZE, "%s %s (Width: %d)", Tools_GetBrushShape() ? "Square" : "Rounded", Tool == SHAPE_LINE ? "Line" : "Rectangle", Tools_GetBrushSize());
		break;
	case SHAPE_CIRCLE:
		snprintf(SelectedToolText, MAX_SELECTEDTOOLTEXT_SIZE, "Circle (Boundary Width: %d)", Tools_GetBrushSize());
		break;
	case TOOL_FLOODFILL:
	case TOOL_PAN:
		snprintf(SelectedToolText, MAX_SELECTEDTOOLTEXT_SIZE, "%s", Tool == TOOL_FLOODFILL ? "Flood Fill" : "Panning");
		break;
	case TOOL_INKDROPPER:
		snprintf(SelectedToolText, MAX_SELECTEDTOOLTEXT_SIZE, "Ink Dropper");
		break;
	}
}

static void _GuiSetColors(ImGuiStyle& style) {
	if (ThemeArr != NULL && ThemeArr->entries[ThemeIndex] != NULL) {
		theme_t* t = ThemeArr->entries[ThemeIndex];
		style = t->style;
	}
}

// Basically Checks if Canvas Is Not Locked, Current Layer isn't NULL, Mouse Positons Are In Bound
static inline bool CanMutateCanvas() {
	return (
		!CanvasLocked                   &&
		CURR_CANVAS_LAYER != NULL       &&
		MousePosRel[0] >= 0             &&
		MousePosRel[0] < CanvasDims[0]  &&
		MousePosRel[1] >= 0             &&
		MousePosRel[1] < CanvasDims[1]
	);
}

// Drawing And Stuff Is Done Here
static void MutateCanvas(bool LmbJustReleased) {
	if (CanMutateCanvas() && (LmbJustReleased || IsLMBDown)) {
		switch (Tool) {
			case BRUSH_COLOR:
			case BRUSH_ERASER: {
				CanvasDidMutate = Tool_Line(CURR_CANVAS_LAYER->pixels, Tool == BRUSH_COLOR ? SelectedColor : EraseColor, MousePosRelLast[0], MousePosRelLast[1], MousePosRel[0], MousePosRel[1], CanvasDims[0], CanvasDims[1]) || CanvasDidMutate;
				break;
			}
			case SHAPE_RECT:
			case SHAPE_CIRCLE:
			case SHAPE_LINE: {
				if (LmbJustReleased) {
					CanvasDidMutate = CanvasDidMutate || (Tool == SHAPE_LINE || Tool == SHAPE_RECT || Tool == SHAPE_CIRCLE);
					if (CanvasDidMutate == true) {
						SaveHistory(&CURR_CANVAS_LAYER->history, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t), CURR_CANVAS_LAYER->pixels);
						CanvasDidMutate = false;
					}
				} else if (IsLMBDown) {
					if (CURR_CANVAS_LAYER->history->prev != NULL) {
						memcpy(CURR_CANVAS_LAYER->pixels, CURR_CANVAS_LAYER->history->pixels, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t));
					} else {
						memset(CURR_CANVAS_LAYER->pixels, 0, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t));
					}
					if (Tool == SHAPE_RECT) {
						Tool_Rect(CURR_CANVAS_LAYER->pixels, SelectedColor, MousePosDownRel[0], MousePosDownRel[1], MousePosRel[0], MousePosRel[1], CanvasDims[0], CanvasDims[1]);
					} else if (Tool == SHAPE_LINE) {
						Tool_Line(CURR_CANVAS_LAYER->pixels, SelectedColor, MousePosDownRel[0], MousePosDownRel[1], MousePosRel[0], MousePosRel[1], CanvasDims[0], CanvasDims[1]);
					} else if (Tool == SHAPE_CIRCLE) {
						Tool_Circle(
							CURR_CANVAS_LAYER->pixels,
							SelectedColor,
							MousePosDownRel[0], MousePosDownRel[1],
							(int)sqrt( // Calculates Distance Between 2 x, y points
								(MousePosRel[0] - MousePosDownRel[0]) * (MousePosRel[0] - MousePosDownRel[0]) +
								(MousePosRel[1] - MousePosDownRel[1]) * (MousePosRel[1] - MousePosDownRel[1])
							),
							CanvasDims[0], CanvasDims[1]
						);
					}
				}
				break;
			}
			case TOOL_PAN: {
				break;
			}
			case TOOL_FLOODFILL: {
				if (LmbJustReleased) {
					unsigned char* pixel = GetCharData(CURR_CANVAS_LAYER->pixels, MousePosRel[0], MousePosRel[1], CanvasDims[0], CanvasDims[1]);
					unsigned char OldColor[4] = { *(pixel + 0), *(pixel + 1), *(pixel + 2), *(pixel + 3) };
					CanvasDidMutate = Tool_FloodFill(
						CURR_CANVAS_LAYER->pixels,
						OldColor, SelectedColor,
						MousePosRel[0], MousePosRel[1],
						CanvasDims[0], CanvasDims[1]
					) || CanvasDidMutate;
				}
				break;
			}
			case TOOL_INKDROPPER: {
				if (LmbJustReleased) {
					uint8_t* pixel = GetPixel(MousePosRel[0], MousePosRel[1]);
					if (pixel != NULL && *(pixel + 3) != 0) {
						bool foundEntry = false;
						for (unsigned int i = 0; i < GetSelectedPalette()->numOfEntries; i++) {
							if (COLOR_EQUAL(GetSelectedPalette()->Colors[i], pixel)) {
								PaletteColorIndex = i;
								SelectedColor[0] = GetSelectedPalette()->Colors[PaletteColorIndex][0];
								SelectedColor[1] = GetSelectedPalette()->Colors[PaletteColorIndex][1];
								SelectedColor[2] = GetSelectedPalette()->Colors[PaletteColorIndex][2];
								SelectedColor[3] = GetSelectedPalette()->Colors[PaletteColorIndex][3];
								foundEntry = true;
								break;
							}
						}
						if (!foundEntry) {
							SelectedColor[0] = *(pixel + 0);
							SelectedColor[1] = *(pixel + 1);
							SelectedColor[2] = *(pixel + 2);
							SelectedColor[3] = *(pixel + 3);
						}
						Tool = LastTool;
						_GuiSetToolText();
					}
				}
				break;
			}
		}
	}
}

static inline void OnEvent_KeyUp(SDL_Event* e) {
	switch(e->key.keysym.sym) {
		case SDLK_SPACE:
			Tool = LastTool;
			CanvasMutable = true;
			_GuiSetToolText();
			break;
		case SDLK_f:
			Tool = TOOL_FLOODFILL;
			_GuiSetToolText();
			break;
		case SDLK_e:
			Tool = BRUSH_ERASER;
			Tools_SetBrushShape(IsShiftDown ? BRUSH_SHAPE_SQUARE : BRUSH_SHAPE_CIRCLE);
			_GuiSetToolText();
			break;
		case SDLK_b:
			Tool = BRUSH_COLOR;
			Tools_SetBrushShape(IsShiftDown ? BRUSH_SHAPE_SQUARE : BRUSH_SHAPE_CIRCLE);
			_GuiSetToolText();
			break;
		case SDLK_l:
			Tool = SHAPE_LINE;
			Tools_SetBrushShape(IsShiftDown ? BRUSH_SHAPE_SQUARE : BRUSH_SHAPE_CIRCLE);
			_GuiSetToolText();
			break;
		case SDLK_r:
			Tool = SHAPE_RECT;
			Tools_SetBrushShape(IsShiftDown ? BRUSH_SHAPE_SQUARE : BRUSH_SHAPE_CIRCLE);
			_GuiSetToolText();
			break;
		case SDLK_c:
			Tool = SHAPE_CIRCLE;
			_GuiSetToolText();
			break;
		case SDLK_s:
			if (IsCtrlDown == true) {
				if (IsShiftDown) {
					ShowSaveAsFileWindow = true;
				} else {
					ifio_write(FilePath, CanvasDims[0], CanvasDims[1], CanvasLayers);
				}
			}
			break;
		case SDLK_o:
			if (IsCtrlDown == true) {
				ShowOpenNewFileWindow = true;
			}
			break;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			IsCtrlDown = false;
			break;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			IsShiftDown = false;
			break;
	}
}

static inline void OnEvent_KeyDown(SDL_Event* e) {
	switch(e->key.keysym.sym) {
		case SDLK_SPACE:
			if (Tool != TOOL_PAN) {
				LastTool = Tool;
				Tool = TOOL_PAN;
				CanvasMutable = false;
				_GuiSetToolText();
			}
			break;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			IsCtrlDown = true;
			break;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			IsShiftDown = true;
			break;
		case SDLK_z:
			if (IsCtrlDown && CURR_CANVAS_LAYER != NULL) {
				UNDO();
			}
			break;
		case SDLK_y:
			if (IsCtrlDown && CURR_CANVAS_LAYER != NULL) {
				REDO();
			}
			break;
		case SDLK_i:
			if (Tool != TOOL_INKDROPPER) {
				LastTool = Tool;
				Tool = TOOL_INKDROPPER;
				_GuiSetToolText();
			}
			break;
	}
}

static inline void OnEvent_MouseWheel(SDL_Event* e) {
	if (IsCtrlDown == true) {
		if (e->wheel.y > 0) { // Scroll Up - Zoom In
			ZoomViewport(1);
		} else if (e->wheel.y < 0) { // Scroll Down - Zoom Out
			ZoomViewport(0);
		}
	} else {
		uint32_t BrushSize = Tools_GetBrushSize();
		if (e->wheel.y > 0) {
			BrushSize++;
		} else if (e->wheel.y < 0) {
			if (BrushSize > 1) {
				BrushSize--;
			}
		}
		Tools_SetBrushSize(BrushSize);
		_GuiSetToolText();
	}
}

static inline void OnEvent_MouseMotion(SDL_Event* e) {
	MousePosLast[0] = MousePos[0];
	MousePosLast[1] = MousePos[1];
	MousePos[0] = e->motion.x;
	MousePos[1] = e->motion.y;

	MousePosRelLast[0] = MousePosRel[0];
	MousePosRelLast[1] = MousePosRel[1];
	MousePosRel[0] = (MousePos[0] - ViewportLoc.x) / CurrViewportZoom;
	MousePosRel[1] = (MousePos[1] - ViewportLoc.y) / CurrViewportZoom;

	if (Tool == TOOL_PAN) {
		ViewportLoc.x -= MousePosLast[0] - MousePos[0];
		ViewportLoc.y -= MousePosLast[1] - MousePos[1];
	}

	MutateCanvas(false);
}

static inline void OnEvent_MouseButtonDown(SDL_Event* e) {
	if (e->button.button == SDL_BUTTON_LEFT) {
		MousePosDown[0] = MousePos[0];
		MousePosDown[1] = MousePos[1];
		MousePosDownRel[0] = MousePosRel[0];
		MousePosDownRel[1] = MousePosRel[1];

		IsLMBDown = true;
		if (CanMutateCanvas()) MutateCanvas(false);
	}
}

static inline void OnEvent_MouseButtonUp(SDL_Event* e) {
	if (e->button.button == SDL_BUTTON_LEFT) {
		IsLMBDown = false;
		if (CanMutateCanvas()) MutateCanvas(true);
	}
}

static inline void ProcessEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type) {
			case SDL_QUIT:
				ShouldClose = true;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
					ShouldClose = true;
				}
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					int w = 0, h = 0;
					SDL_GetWindowSize(window, &w, &h);
					if (w > 0 && h > 0) {
						WindowDims[0] = w;
						WindowDims[1] = h;
					} else {
						log_error("invalid window size %dx%d", w, h);
					}
				}
				break;
			case SDL_KEYDOWN:
				OnEvent_KeyDown(&event);
				break;
			case SDL_KEYUP:
				OnEvent_KeyUp(&event);
				break;
			case SDL_MOUSEWHEEL:
				OnEvent_MouseWheel(&event);
				break;
			case SDL_MOUSEMOTION:
				OnEvent_MouseMotion(&event);
				break;
			case SDL_MOUSEBUTTONDOWN:
				OnEvent_MouseButtonDown(&event);
				break;
			case SDL_MOUSEBUTTONUP:
				OnEvent_MouseButtonUp(&event);
				break;
		}
	}

	if (CanvasDidMutate == true && IsLMBDown == false) {
		SaveHistory(&CURR_CANVAS_LAYER->history, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t), CURR_CANVAS_LAYER->pixels);
		CanvasDidMutate = false;
	}
}

static void UpdateViewportSize() {
	ViewportLoc.w = (int)CanvasDims[0] * CurrViewportZoom;
	ViewportLoc.h = (int)CanvasDims[1] * CurrViewportZoom;
}

static void UpdateViewportPos() {
	ViewportLoc.x = (int)(WindowDims[0] / 2) - (CanvasDims[0] * CurrViewportZoom / 2);
	ViewportLoc.y = (int)(WindowDims[1] / 2) - (CanvasDims[1] * CurrViewportZoom / 2);
}

static void ZoomViewport(int increase) {
	if (CanvasLocked) return;
	if (increase > 0) {
		if (CurrViewportZoom < 1.0f) CurrViewportZoom += 0.25f;
		else CurrViewportZoom += 1.0f;
	} else {
		if (CurrViewportZoom > 0.1f) {
			if (CurrViewportZoom < 1.0f) CurrViewportZoom -= 0.25f;
			else CurrViewportZoom -= 1.0f;
		}
	}
	if (CurrViewportZoom < 0.5f) CurrViewportZoom = 0.5f;
	else if (CurrViewportZoom > 100.0f) CurrViewportZoom = 100.0f;

	// This Ensures That The Canvas Is Zoomed From It's Center And Not From The Bottom Left Position
	int32_t CurrRectCenter[2] = { (ViewportLoc.w / 2) + ViewportLoc.x, (ViewportLoc.h / 2) + ViewportLoc.y };
	int32_t NewRectCenter[2] = {
		(int32_t)(CanvasDims[0] * CurrViewportZoom / 2) + ViewportLoc.x,
		(int32_t)(CanvasDims[1] * CurrViewportZoom / 2) + ViewportLoc.y
	};
	ViewportLoc.x -= NewRectCenter[0] - CurrRectCenter[0];
	ViewportLoc.y -= NewRectCenter[1] - CurrRectCenter[1];
	UpdateViewportSize();
}

static uint8_t* GetPixel(int x, int y) {
	if (CURR_CANVAS_LAYER == NULL) return NULL;
	return CURR_CANVAS_LAYER->pixels + ((y * CanvasDims[0] + x) * 4);
}

static void InitWindowIcon() {
	uint8_t* winIcon = (uint8_t*)Assets_Get("data/icons/icon-24.png", NULL);
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		winIcon,
		24, 24, 32, 24 * 4,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000
	);
	if (surface == NULL) {
		log_error("Failed to set window icon: %s", SDL_GetError());
		return;
	}
	SDL_SetWindowIcon(window, surface);
	SDL_FreeSurface(surface);
}

static int OpenNewFile(const char* _fName) {
	if (_fName == NULL) {
		log_error("_fName is NULL!");
		return -1;
	}

	int32_t w = 0, h = 0;
	if (ifio_read(_fName, &w, &h, &CanvasLayers) == 0 && w > 0 && h > 0) {
		CanvasDims[0] = w;
		CanvasDims[1] = h;
		CurrViewportZoom = 1.0f;
		SelectedLayerIndex = 0;
		UpdateViewportSize();
		UpdateViewportPos();

		snprintf(FilePath, SYS_PATHNAME_MAX, "%s", _fName);
		char* filePathBasename = Sys_GetBasename(_fName);
		snprintf(FileName, SYS_FILENAME_MAX, "%s", filePathBasename);
		free(filePathBasename);
		UPDATE_WINDOW_TITLE();
	}
	return 0;
}

