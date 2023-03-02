#if defined(_WIN32) && defined(_MSC_VER)
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

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
bool ShowAboutWindow = false;
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

int32_t ThemeIndex = 0;

#define MAX_SELECTEDTOOLTEXT_SIZE 512
char SelectedToolText[MAX_SELECTEDTOOLTEXT_SIZE] = "";

SDL_Window* window = NULL;
#define WINDOW_TITLE_MAX 512 + SYS_FILENAME_MAX
char WindowTitle[WINDOW_TITLE_MAX] = "";

PaletteManager* pMgr      = NULL;
theme_arr_t*    ThemeArr  = NULL;
Config_T*       AppConfig = NULL;

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
		if (FileName[0] != 0 && CanvasLayers != NULL) { snprintf(WindowTitle, WINDOW_TITLE_MAX, "%s (%dx%d) - csprite " VERSION_STR, FileName, CanvasLayers->dims[0], CanvasLayers->dims[1]); }\
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

#define UNDO() \
	if (CURR_CANVAS_LAYER != NULL) HISTORY_UNDO(CURR_CANVAS_LAYER->history, CanvasLayers->dims[0] * CanvasLayers->dims[1] * 4 * sizeof(uint8_t), CURR_CANVAS_LAYER->pixels)

#define REDO() \
	if (CURR_CANVAS_LAYER != NULL) HISTORY_REDO(CURR_CANVAS_LAYER->history, CanvasLayers->dims[0] * CanvasLayers->dims[1] * 4 * sizeof(uint8_t), CURR_CANVAS_LAYER->pixels)

#define UPDATE_WINDOW_TITLE() do {\
		GEN_WIN_TITLE();\
		SDL_SetWindowTitle(window, WindowTitle);\
	} while(0)

int main(int argc, char* argv[]) {
	FILE* LogFilePtr = fopen(Sys_GetLogFileName(), "w");
	log_add_fp(LogFilePtr, LOG_TRACE);

	AppConfig = LoadConfig();
	pMgr = new PaletteManager;

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

	InitWindowIcon();
	SDL_ShowWindow(window);
	_GuiSetToolText();

	if (R_Init(window, AppConfig->vsync) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	SDL_Renderer* renderer = R_GetRenderer();

	CanvasLayers = Canvas_CreateArr(100, 64, 64);
	if (CanvasLayers == NULL) return EXIT_FAILURE;

	CURR_CANVAS_LAYER = Canvas_CreateLayer(renderer, CanvasLayers->dims[0], CanvasLayers->dims[1]);
	if (CURR_CANVAS_LAYER == NULL) return EXIT_FAILURE;
	CanvasLayers->size++;

	UPDATE_WINDOW_TITLE();
	UpdateViewportPos();
	UpdateViewportSize();

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
		ImFontConfig fontConfig;
		fontConfig.RasterizerMultiply = 2.0f;
		int bmMiniFontSize = 0;
		const void* bmMiniFont = Assets_Get("data/fonts/bm-mini.ttf", &bmMiniFontSize);
		if (bmMiniFont) io.Fonts->AddFontFromMemoryCompressedTTF(bmMiniFont, bmMiniFontSize, 16.0f, &fontConfig);
	}

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

#if(CS_BUILD_STABLE == 0)
		static bool metricsWinVisible = false;
		if (metricsWinVisible) {
			ImGui::ShowMetricsWindow(NULL);
		}
#endif

		static ImVec2 MainMenuPos;
		static ImVec2 MainMenuSize;
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
						ifio_write(FilePath, CanvasLayers->dims[0], CanvasLayers->dims[1], CanvasLayers);
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
					for (int32_t i = 0; i < (int32_t)pMgr->presets->size(); ++i) {
						if (ImGui::MenuItem((*pMgr->presets)[i].name.c_str(), NULL)) {
							pMgr->SetPreset((*pMgr->presets)[i]);
						}
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
							ImGui::SetTooltip("%s", (*pMgr->presets)[i].author.c_str());
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
#if(CS_BUILD_STABLE == 0)
				if (ImGui::MenuItem("Metrics")) {
					if (metricsWinVisible) metricsWinVisible = false;
					else metricsWinVisible = true;
				}
#endif
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("Wiki")) {
					Sys_OpenURL("https://csprite.github.io/wiki/");
				}
				if (ImGui::MenuItem("About")) {
					ShowAboutWindow = true;
				}
				if (ImGui::MenuItem("GitHub")) {
					Sys_OpenURL("https://github.com/pegvin/csprite");
				}
				if (ImGui::MenuItem("Contributors")) {
					Sys_OpenURL("https://github.com/pegvin/csprite/graphs/contributors");
				}
				ImGui::EndMenu();
			}

			MainMenuPos = ImGui::GetWindowPos();
			MainMenuSize = ImGui::GetWindowSize();
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

		if (ImFileDialog.showFileDialog("Select a file##Csprite_OpenNewFileDlg", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, { io.DisplaySize.x * 0.9f, io.DisplaySize.y * 0.9f }, ".csprite,.png,.jpg,.jpeg,.bmp")) {
			OpenNewFile(ImFileDialog.selected_path.c_str());
		}
		if (ImFileDialog.showFileDialog("Save file as##Csprite_SaveAsFileDlg", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, { io.DisplaySize.x * 0.9f, io.DisplaySize.y * 0.9f }, ".csprite,.png,.jpg,.jpeg,.bmp")) {
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
			ifio_write(FilePath, CanvasLayers->dims[0], CanvasLayers->dims[1], CanvasLayers);
			free(_fName);
			_fName = NULL;
			_fPath = NULL;
		}

		if (ShowAboutWindow) {
			if (ImGui::BeginPopupModal("About##Csprite_AboutWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Csprite " VERSION_STR);
				ImGui::Separator();
				ImGui::Text("A simple yet feature rich pixel art tool");

				if (ImGui::Button("Close")) {
					ImGui::CloseCurrentPopup();
					ShowAboutWindow = false;
				}
				ImGui::EndPopup();
			} else {
				ImGui::OpenPopup("About##Csprite_AboutWindow");
			}
		}

		static ImVec2 LeftSideBarPos;
		static ImVec2 LeftSideBarSize;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::SetNextWindowSizeConstraints({ 50, -1 }, { (float)WindowDims[0], -1 });
		ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);
		ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, 0);
		ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, 0);
		if (ImGui::Begin("LeftSideBar", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
			LeftSideBarPos = { -1, MainMenuPos.y + MainMenuSize.y };
			LeftSideBarSize = { (float)WindowDims[0] / 8, WindowDims[1] - (MainMenuPos.y + MainMenuSize.y) };

			ImGui::SetWindowPos(LeftSideBarPos);
			ImGui::SetWindowSize(LeftSideBarSize, ImGuiCond_Once);

			ImVec2 newFramePadding = { style.FramePadding.x * 4.0f, style.FramePadding.y * 4.0f };
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, newFramePadding);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			static bool primaryIsInPalette = true;
			for (int32_t i = 0; i < (int32_t)pMgr->palette.colors.size(); i++) {
				ImGui::PushID(i);

				static char ColorButtonId[20] = "";
				if (pMgr->SelectedColorIdx != i) { snprintf(ColorButtonId, 20, "Color##%d", i); }

				if (ImGui::ColorButton(pMgr->SelectedColorIdx == i ? "Selected Color" : ColorButtonId, {
					((float)(pMgr->palette.colors[i].r) / 255),
					((float)(pMgr->palette.colors[i].g) / 255),
					((float)(pMgr->palette.colors[i].b) / 255),
					((float)(pMgr->palette.colors[i].a) / 255)
				})) {
					pMgr->SetSelectedColorIdx(i);
				}

				primaryIsInPalette = primaryIsInPalette || (
					pMgr->PrimaryColor[0] == pMgr->palette.colors[i].r  &&
					pMgr->PrimaryColor[1] == pMgr->palette.colors[i].g  &&
					pMgr->PrimaryColor[2] == pMgr->palette.colors[i].b  &&
					pMgr->PrimaryColor[3] == pMgr->palette.colors[i].a
				);

				ImGui::GetWindowDrawList()->AddRect(
					ImGui::GetItemRectMin(),
					ImGui::GetItemRectMax(),
					(pMgr->SelectedColorIdx == i && primaryIsInPalette) ? 0xFFFFFFFF : 0x000000FF,
					0, 0, 1
				);

				float lastBtnSizeX = ImGui::GetItemRectMax().x;
				float nextBtnSizeX = lastBtnSizeX + style.ItemSpacing.x + ImGui::GetItemRectSize().x; // Expected position if next button was on same line
				if (i + 1 < (int32_t)pMgr->palette.colors.size() && nextBtnSizeX < window_visible_x2) ImGui::SameLine();
				ImGui::PopID();
			};
			ImGui::PopStyleVar(2);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (primaryIsInPalette) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style.Alpha * 0.5f);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_Button]);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_Button]);
			}

			if (ImGui::Button("Add") && !primaryIsInPalette) {
				pMgr->palette.AddColor(
					pMgr->PrimaryColor[0],
					pMgr->PrimaryColor[1],
					pMgr->PrimaryColor[2],
					pMgr->PrimaryColor[3]
				);
			}

			if (primaryIsInPalette) {
				ImGui::PopStyleVar(); // ImGuiStyleVar_Alpha
				ImGui::PopStyleColor(2); // ImGuiCol_ButtonActive, ImGuiCol_ButtonHovered
			}

			ImGui::SameLine();

			/* storing the size makes sure that we for sure pop
			   the pushed styles & colors, as RemoveColor() can change
			   the colors.size() thus affecting the pop operation not being executed */
			int32_t pSize = pMgr->palette.colors.size();
			if (pSize <= 0) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style.Alpha * 0.5f);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_Button]);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_Button]);
			}

			if (ImGui::Button("Remove") && pSize > 0 && primaryIsInPalette) {
				pMgr->palette.RemoveColor(
					pMgr->PrimaryColor[0],
					pMgr->PrimaryColor[1],
					pMgr->PrimaryColor[2],
					pMgr->PrimaryColor[3]
				);
				pMgr->SetSelectedColorIdx(
					CLAMP_NUM(pMgr->SelectedColorIdx, 0, pSize - 1)
				);
			}

			if (pSize <= 0) {
				ImGui::PopStyleVar(); // ImGuiStyleVar_Alpha
				ImGui::PopStyleColor(2); // ImGuiCol_ButtonActive, ImGuiCol_ButtonHovered
			}

			primaryIsInPalette = false;

			float ImColPicker[4] = {
				(float)(pMgr->PrimaryColor[0]) / 255,
				(float)(pMgr->PrimaryColor[1]) / 255,
				(float)(pMgr->PrimaryColor[2]) / 255,
				(float)(pMgr->PrimaryColor[3]) / 255
			};
			ImGui::SetNextItemWidth(-FLT_MIN); // right align
			if (ImGui::ColorPicker4(
				"##ColorPickerWidget", (float*)&ImColPicker,
				ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview
			)) {
				pMgr->PrimaryColor[0] = ImColPicker[0] * 255;
				pMgr->PrimaryColor[1] = ImColPicker[1] * 255;
				pMgr->PrimaryColor[2] = ImColPicker[2] * 255;
				pMgr->PrimaryColor[3] = ImColPicker[3] * 255;
			}

			LeftSideBarSize = ImGui::GetWindowSize();
			ImGui::End();
		}

		static ImVec2 TopBarSize, TopBarPos;
		if (ImGui::Begin("TopBar", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			ImGui::Text("%s", SelectedToolText);
			if (ImGui::BeginPopupContextItem("##TopBarPopupContext1")) {
				if (ImGui::BeginMenu("Brush")) {
					if (ImGui::MenuItem("Square")) { Tools_SetBrushShape(BRUSH_SHAPE_SQUARE); }
					if (ImGui::MenuItem("Circle")) { Tools_SetBrushShape(BRUSH_SHAPE_CIRCLE); }
					ImGui::EndMenu();
				}
				int32_t bSize = Tools_GetBrushSize();
				if (ImGui::SliderInt("##BrushSize", &bSize, 1, CanvasLayers->dims[0])) {
					Tools_SetBrushSize(bSize);
				}
				ImGui::EndPopup();
			}

			TopBarPos = { LeftSideBarPos.x + LeftSideBarSize.x, MainMenuPos.y + MainMenuSize.y };
			TopBarSize = { (float)WindowDims[0] - ImGui::GetWindowPos().x, ImGui::GetTextLineHeightWithSpacing() };
			ImGui::SetWindowPos(TopBarPos);
			ImGui::SetWindowSize(TopBarSize);
			ImGui::End();
		}

		static ImVec2 BottomBarSize, BottomBarPos;
		if (ImGui::Begin("BottomBar", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			ImGui::Text("Zoom: %d%%", (int)(CurrViewportZoom * 100));

			BottomBarPos = { LeftSideBarPos.x + LeftSideBarSize.x, (float)WindowDims[1] - (ImGui::GetTextLineHeightWithSpacing() + style.WindowPadding.y + style.FramePadding.y) };
			BottomBarSize = { (float)WindowDims[0] - ImGui::GetWindowPos().x, ImGui::GetTextLineHeightWithSpacing() + style.WindowPadding.y + style.FramePadding.y };
			ImGui::SetWindowPos(BottomBarPos);
			ImGui::SetWindowSize(BottomBarSize);
			ImGui::End();
		}
		ImGui::PopStyleVar();

		if (ImGui::Begin(
			"Debug Window", NULL,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize
#if(CS_BUILD_STABLE == 1)
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground
#endif
		)) {
			ImVec2 DebugWinSize = ImGui::GetWindowSize();
			ImGui::SetWindowPos({ ((float)WindowDims[0] / 2) - (DebugWinSize.x / 2), io.DisplaySize.y - DebugWinSize.y - BottomBarSize.y });
#if(CS_BUILD_STABLE == 0)
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			if (ImGui::Button("Clear Undo/Redo Buffers")) {
				for (int i = 0; i < CanvasLayers->size; ++i) {
					if (CanvasLayers->layers[i] != NULL) {
						FreeHistory(&CanvasLayers->layers[i]->history);
						SaveHistory(&CanvasLayers->layers[i]->history, CanvasLayers->dims[0] * CanvasLayers->dims[1] * 4 * sizeof(uint8_t), CanvasLayers->layers[i]->pixels);
					}
				}
			}
#endif
			ImGui::End();
		}

		static int32_t PreviewZoom = 2;
		static ImVec2  PreviewImageSize;
		static bool ReCalculateZoomSize = true;

		if (ShowCanvasPreviewWindow && ImGui::Begin("Preview", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::BeginMenu("Zoom")) {
					if (ImGui::MenuItem("Increase")) { PreviewZoom++; ReCalculateZoomSize = true; }
					if (ImGui::MenuItem("Decrease")) { if (PreviewZoom > 1) { PreviewZoom--; ReCalculateZoomSize = true; } }
					ImGui::EndMenu();
				}
				ImGui::EndPopup();
			}

			if (ReCalculateZoomSize) {
				PreviewImageSize.x = CanvasLayers->dims[0] * PreviewZoom;
				PreviewImageSize.y = CanvasLayers->dims[1] * PreviewZoom;
				ReCalculateZoomSize = false;
			}

			ImGui::SetWindowPos({ io.DisplaySize.x - ImGui::GetWindowSize().x - 10, io.DisplaySize.y - ImGui::GetWindowSize().y - (BottomBarSize.y + 10) });
			ImGui::Image(reinterpret_cast<ImTextureID>(CanvasLayers->renderTex), PreviewImageSize);
			ImGui::End();
		}

		static ImVec2 LayerWinSize;
		ImGui::SetNextWindowSizeConstraints({ 200, 300 }, { io.DisplaySize.y / 1.5f, io.DisplaySize.y / 2.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		if (ImGui::Begin("Layer", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar)) {
			if (ImGui::Button("+")) {
				if (SelectedLayerIndex + 1 >= CanvasLayers->capacity) {
					int32_t newSize = CanvasLayers->capacity + 50;
					Canvas_ResizeArr(CanvasLayers, newSize);
					if (CanvasLayers->capacity == newSize) {
						SelectedLayerIndex++;
						CURR_CANVAS_LAYER = Canvas_CreateLayer(renderer, CanvasLayers->dims[0], CanvasLayers->dims[1]);
						CanvasLayers->size++;
						log_info("Resized canvas layers array to %d", newSize);
					} else {
						log_error("Unable to resize canvas layers array!");
					}
				} else {
					if (CURR_CANVAS_LAYER != NULL) SelectedLayerIndex++; // when opening a .csprite file with no layers CURR_CANVAS_LAYER might be NULL
					CURR_CANVAS_LAYER = Canvas_CreateLayer(renderer, CanvasLayers->dims[0], CanvasLayers->dims[1]);
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

			LayerWinSize = ImGui::GetWindowSize();
			ImGui::SetWindowSize(LayerWinSize, ImGuiCond_Once);
			ImGui::SetWindowPos({ io.DisplaySize.x - LayerWinSize.x + 1, TopBarPos.y + TopBarSize.y + style.WindowPadding.y + style.FramePadding.y });
			ImGui::End();
		}
		ImGui::PopStyleVar(); // ImGuiStyleVar_WindowBorderSize
		ImGui::PopStyleColor(3); // ImGuiCol_ResizeGrip, ImGuiCol_ResizeGripHovered, ImGuiCol_ResizeGripActive

		if (ShowNewCanvasWindow) {
			if (ImGui::BeginPopupModal("Create New###NewCanvasWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
				static int32_t NewDims[2] = { 64, 64 };
				ImGui::InputInt("Width", &NewDims[0], 1, 5);
				ImGui::InputInt("Height", &NewDims[1], 1, 5);

				if (ImGui::Button("Create")) {
					if (NewDims[0] > 0 && NewDims[1] > 0) {
						Canvas_DestroyArr(CanvasLayers);
						CanvasLayers = Canvas_CreateArr(100, NewDims[0], NewDims[1]);
						SelectedLayerIndex = 0;
						CURR_CANVAS_LAYER = Canvas_CreateLayer(renderer, NewDims[0], NewDims[1]);
						CanvasLayers->size++;
						CurrViewportZoom = 1.0f;
						UPDATE_WINDOW_TITLE();
						UpdateViewportSize();
						UpdateViewportPos();
					}
					ShowNewCanvasWindow = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					NewDims[0] = 64;
					NewDims[1] = 64;
					ShowNewCanvasWindow = false;
					ImGui::CloseCurrentPopup();
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
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("please restart the app after saving");
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					ShowPreferencesWindow = false;
					ImGui::CloseCurrentPopup();
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
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					memset(TempBuff, 0, LAYER_NAME_MAX);
					ShowLayerRenameWindow = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			} else {
				ImGui::OpenPopup("Rename Layer###LayerRenameWindow");
				isFirstTime = true; // set to true cause next frame the popup will appear.
			}
		}

		if (CanvasLayers->size > 0) {
			Canvas_Draw(renderer, CanvasLayers, &ViewportLoc, SelectedLayerIndex);
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
	R_Destroy();
	SDL_DestroyWindow(window);
	SDL_Quit();
	delete pMgr;

	window = NULL;
	pMgr = NULL;
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
		MousePosRel[0] < CanvasLayers->dims[0]  &&
		MousePosRel[1] >= 0             &&
		MousePosRel[1] < CanvasLayers->dims[1]
	);
}

// Drawing And Stuff Is Done Here
static void MutateCanvas(bool LmbJustReleased) {
	if (CanMutateCanvas() && (LmbJustReleased || IsLMBDown)) {
		static uint8_t EraseColor[4] = { 0, 0, 0, 0 };

		switch (Tool) {
			case BRUSH_COLOR:
			case BRUSH_ERASER: {
				CanvasDidMutate = Tool_Line(CURR_CANVAS_LAYER->pixels, Tool == BRUSH_COLOR ? pMgr->PrimaryColor : EraseColor, MousePosRelLast[0], MousePosRelLast[1], MousePosRel[0], MousePosRel[1], CanvasLayers->dims[0], CanvasLayers->dims[1]) || CanvasDidMutate;
				break;
			}
			case SHAPE_RECT:
			case SHAPE_CIRCLE:
			case SHAPE_LINE: {
				if (LmbJustReleased) {
					CanvasDidMutate = CanvasDidMutate || (Tool == SHAPE_LINE || Tool == SHAPE_RECT || Tool == SHAPE_CIRCLE);
					if (CanvasDidMutate == true) {
						SaveHistory(&CURR_CANVAS_LAYER->history, CanvasLayers->dims[0] * CanvasLayers->dims[1] * 4 * sizeof(uint8_t), CURR_CANVAS_LAYER->pixels);
						CanvasDidMutate = false;
					}
				} else if (IsLMBDown) {
					if (CURR_CANVAS_LAYER->history->prev != NULL) {
						memcpy(CURR_CANVAS_LAYER->pixels, CURR_CANVAS_LAYER->history->pixels, CanvasLayers->dims[0] * CanvasLayers->dims[1] * 4 * sizeof(uint8_t));
					} else {
						memset(CURR_CANVAS_LAYER->pixels, 0, CanvasLayers->dims[0] * CanvasLayers->dims[1] * 4 * sizeof(uint8_t));
					}
					if (Tool == SHAPE_RECT) {
						Tool_Rect(CURR_CANVAS_LAYER->pixels, pMgr->PrimaryColor, MousePosDownRel[0], MousePosDownRel[1], MousePosRel[0], MousePosRel[1], CanvasLayers->dims[0], CanvasLayers->dims[1]);
					} else if (Tool == SHAPE_LINE) {
						Tool_Line(CURR_CANVAS_LAYER->pixels, pMgr->PrimaryColor, MousePosDownRel[0], MousePosDownRel[1], MousePosRel[0], MousePosRel[1], CanvasLayers->dims[0], CanvasLayers->dims[1]);
					} else if (Tool == SHAPE_CIRCLE) {
						Tool_Circle(
							CURR_CANVAS_LAYER->pixels,
							pMgr->PrimaryColor,
							MousePosDownRel[0], MousePosDownRel[1],
							(int)sqrt( // Calculates Distance Between 2 x, y points
								(MousePosRel[0] - MousePosDownRel[0]) * (MousePosRel[0] - MousePosDownRel[0]) +
								(MousePosRel[1] - MousePosDownRel[1]) * (MousePosRel[1] - MousePosDownRel[1])
							),
							CanvasLayers->dims[0], CanvasLayers->dims[1]
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
					unsigned char* pixel = GetCharData(CURR_CANVAS_LAYER->pixels, MousePosRel[0], MousePosRel[1], CanvasLayers->dims[0], CanvasLayers->dims[1]);
					unsigned char OldColor[4] = { *(pixel + 0), *(pixel + 1), *(pixel + 2), *(pixel + 3) };
					CanvasDidMutate = Tool_FloodFill(
						CURR_CANVAS_LAYER->pixels,
						OldColor, pMgr->PrimaryColor,
						MousePosRel[0], MousePosRel[1],
						CanvasLayers->dims[0], CanvasLayers->dims[1]
					) || CanvasDidMutate;
				}
				break;
			}
			case TOOL_INKDROPPER: {
				if (LmbJustReleased) {
					uint8_t* pixel = GetPixel(MousePosRel[0], MousePosRel[1]);
					if (pixel != NULL && *(pixel + 3) != 0) {
						for (unsigned int i = 0; i < pMgr->palette.colors.size(); i++) {
							if (
								pMgr->PrimaryColor[0] == pixel[0] &&
								pMgr->PrimaryColor[1] == pixel[1] &&
								pMgr->PrimaryColor[2] == pixel[2] &&
								pMgr->PrimaryColor[3] == pixel[3]
							) {
								pMgr->SetSelectedColorIdx(i);
								break;
							}
						}
						pMgr->PrimaryColor[0] = *(pixel + 0);
						pMgr->PrimaryColor[1] = *(pixel + 1);
						pMgr->PrimaryColor[2] = *(pixel + 2);
						pMgr->PrimaryColor[3] = *(pixel + 3);

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
					ifio_write(FilePath, CanvasLayers->dims[0], CanvasLayers->dims[1], CanvasLayers);
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
		SaveHistory(&CURR_CANVAS_LAYER->history, CanvasLayers->dims[0] * CanvasLayers->dims[1] * 4 * sizeof(uint8_t), CURR_CANVAS_LAYER->pixels);
		CanvasDidMutate = false;
	}
}

static void UpdateViewportSize() {
	ViewportLoc.w = (int)CanvasLayers->dims[0] * CurrViewportZoom;
	ViewportLoc.h = (int)CanvasLayers->dims[1] * CurrViewportZoom;
}

static void UpdateViewportPos() {
	ViewportLoc.x = (int)(WindowDims[0] / 2) - (CanvasLayers->dims[0] * CurrViewportZoom / 2);
	ViewportLoc.y = (int)(WindowDims[1] / 2) - (CanvasLayers->dims[1] * CurrViewportZoom / 2);
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
		(int32_t)(CanvasLayers->dims[0] * CurrViewportZoom / 2) + ViewportLoc.x,
		(int32_t)(CanvasLayers->dims[1] * CurrViewportZoom / 2) + ViewportLoc.y
	};
	ViewportLoc.x -= NewRectCenter[0] - CurrRectCenter[0];
	ViewportLoc.y -= NewRectCenter[1] - CurrRectCenter[1];
	UpdateViewportSize();
}

static uint8_t* GetPixel(int x, int y) {
	if (CURR_CANVAS_LAYER == NULL) return NULL;
	return CURR_CANVAS_LAYER->pixels + ((y * CanvasLayers->dims[0] + x) * 4);
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

