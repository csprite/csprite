#include "app/app.h"
#include "app/window.h"
#include "cimgui.h"
#include "cimgui_impl.h"
#include "assets/assets.h"
#include "log/log.h"

int AppInit(void) {
	// Initialize Window & ImGui
	if (WindowCreate("csprite", 320, 240, 1)) {
		return 1;
	}

	// Initialize Font
	int fontDataSize = 0;
	ImVector_ImWchar FontRanges;
	const ImGuiIO* io = igGetIO();
	ImVector_ImWchar_Init(&FontRanges);
	ImFontGlyphRangesBuilder *FontBuilder = ImFontGlyphRangesBuilder_ImFontGlyphRangesBuilder();
	ImFontGlyphRangesBuilder_AddRanges(FontBuilder, ImFontAtlas_GetGlyphRangesDefault(io->Fonts));
	ImFontGlyphRangesBuilder_BuildRanges(FontBuilder, &FontRanges);
	ImFontAtlas_AddFontFromMemoryCompressedTTF(
	    io->Fonts, assets_get("data/fonts/Inter.ttf", &fontDataSize),
		fontDataSize, 18, NULL, FontRanges.Data
	);
	ImFontAtlas_Build(io->Fonts);
	if (!ImFontAtlas_IsBuilt(io->Fonts)) {
		log_error("io.Fonts->Build() - failed to build the font atlas");
		return 1;
	}
	ImFontGlyphRangesBuilder_destroy(FontBuilder);
	ImVector_ImWchar_UnInit(&FontRanges);

	// Initialize Styles
	ImGuiStyle *style = igGetStyle();
	#define ImColor(r, g, b) (ImVec4){ r/255.0f, g/255.0f, b/255.0f, 1 }
	style->Colors[ImGuiCol_Button] = ImColor(44, 44, 44);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(55, 55, 55);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(66, 66, 66);

	style->Colors[ImGuiCol_TitleBg] = ImColor(0xFF, 0xAA, 0xFF);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(0xAA, 0x55, 0xAA);

	style->Colors[ImGuiCol_CheckMark] = ImColor(0xAA, 0xAA, 0xAA);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(0xAA, 0xAA, 0xAA);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(0xFF, 0xFF, 0xFF);

	style->Colors[ImGuiCol_FrameBg] = ImColor(44, 44, 44);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(55, 55, 55);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(66, 66, 66);

	style->Colors[ImGuiCol_Header] = ImColor(55, 55, 55);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(66, 66, 66);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(77, 77, 77);

	WindowSetBG(
		style->Colors[ImGuiCol_WindowBg].x * 255.0f,
		style->Colors[ImGuiCol_WindowBg].y * 255.0f,
		style->Colors[ImGuiCol_WindowBg].z * 255.0f
	);
	WindowSetMaxFPS(60);

	return 0;
}

#include "sfd.h"
#include "app/editor.h"

static void _MainWinProcessInput(editor_t* ed);

int AppMainLoop(void) {
	WindowNewFrame();
	WindowEndFrame();

	ImGuiIO* io = igGetIO();

	editor_t ed = {0};
	EditorInit(&ed, 320, 240);
	ed.view.x = (io->DisplaySize.x / 2) - (ed.view.w / 2);
	ed.view.y = (io->DisplaySize.y / 2) - (ed.view.h / 2);

	while (!WindowShouldClose()) {
		WindowNewFrame();

		ImVec2 mBarPos;
		ImVec2 mBarSize;
		igBeginMainMenuBar();
			if (igBeginMenu("File", true)) {
				if (igMenuItem_Bool("Open", NULL, false, true)) {
					const char* filePath = sfd_open_dialog(&(sfd_Options){
						.title        = "Open Image File",
						.filter_name  = "Image File",
						.filter       = "*.png|*.jpg",
						.save         = 0
					});
					if (filePath) {
						editor_t new = {0};
						if (!EditorInitFrom(&new, filePath)) {
							EditorDestroy(&ed);
							ed = new;
							ed.view.x = (io->DisplaySize.x / 2) - (ed.view.w / 2);
							ed.view.y = (io->DisplaySize.y / 2) - (ed.view.h / 2);
						}
					} else {
						const char* LastError = sfd_get_error();
						if (LastError != NULL) {
							printf("Failed to launch dialog: %s", LastError);
						}
					}
				}
				igEndMenu();
			}
			if (igBeginMenu("Help", true)) {
				if (igMenuItem_Bool("About", NULL, false, true)) {
					AppOpenURL("https://csprite.github.io");
				}
				igEndMenu();
			}

			ImVec2 winSize, textSize;
			igGetWindowSize(&winSize);
			igCalcTextSize(&textSize, ed.file.name, NULL, false, -1);
			igSetCursorPosX((winSize.x - textSize.x) * 0.5);
			igText("%s", ed.file.name);

			igGetWindowPos(&mBarPos);
			igGetWindowSize(&mBarSize);
		igEndMainMenuBar();

		static ImVec2 SidebarPos, SidebarSize;
		SidebarSize.y = io->DisplaySize.y - (mBarPos.y + mBarSize.y); // Used as Constraint & To Reduce Duplicate Calculations
		igSetNextWindowPos((ImVec2){ 0, mBarPos.y + mBarSize.y }, ImGuiCond_Always, (ImVec2){ 0, 0 });
		igSetNextWindowSize((ImVec2){ 200, 0 }, ImGuiCond_Once);
		igSetNextWindowSizeConstraints((ImVec2){ 40, SidebarSize.y }, (ImVec2){ io->DisplaySize.x / 3, SidebarSize.y }, NULL, NULL);
		igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0);
		if (igBegin("##Sidebar", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar)) {
			igGetWindowPos(&SidebarPos);
			igGetWindowSize(&SidebarSize);
			igEnd();
		}
		igPopStyleVar(1);

		ImVec2 statusBarPos, statusBarSize;
		igSetNextWindowPos((ImVec2){ SidebarPos.x + SidebarSize.x, mBarPos.y + mBarSize.y }, ImGuiCond_Always, (ImVec2){ 0, 0 });
		igSetNextWindowSize((ImVec2){ io->DisplaySize.x, 0 }, ImGuiCond_Always);
		if (igBegin("##StatusBar", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize)) {
			igSetNextItemWidth(150);
			igSliderInt("##BrushSize", (int*)&ed.tool.brush.size, 1, 64, "%d", 0);
			igSameLine(0, -1);

			igCheckbox("Rounded", &ed.tool.brush.rounded);
			igSameLine(0, -1);

			igText("- Zoom: %.2f", ed.view.scale);

			igGetWindowPos(&statusBarPos);
			igGetWindowSize(&statusBarSize);
			igEnd();
		}

		bool isMainWindowHovered = false;
		igSetNextWindowPos((ImVec2){ SidebarPos.x + SidebarSize.x, statusBarPos.y + statusBarSize.y }, ImGuiCond_Always, (ImVec2){ 0, 0 });
		igSetNextWindowSize((ImVec2){ io->DisplaySize.x, io->DisplaySize.y - (statusBarPos.y + statusBarSize.y) + 1 }, ImGuiCond_Always);
		if (igBegin("Main", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
			ImDrawList_AddRect(
			    igGetWindowDrawList(),
			    (ImVec2){ ed.view.x, ed.view.y }, (ImVec2){ ed.view.x + ed.view.w, ed.view.y + ed.view.h },
			    igGetColorU32_Col(ImGuiCol_Border, 1), 0, 0, 1
			);
			ImDrawList_AddImage(
			    igGetWindowDrawList(), (ImTextureID)ed.canvas.texture,
			    (ImVec2){ ed.view.x, ed.view.y }, (ImVec2){ ed.view.x + ed.view.w, ed.view.y + ed.view.h },
			    (ImVec2){ 0, 0 }, (ImVec2){ 1, 1 }, 0xFFFFFFFF
			);
			isMainWindowHovered = igIsWindowHovered(0);
			igEnd();
		}

		mmRect_t dirtyArea = {0};
		mmRect_t totalDirty = { ed.canvas.image.width, ed.canvas.image.height, 0, 0 };
		if (isMainWindowHovered) {
			_MainWinProcessInput(&ed);

			if (igIsMouseClicked_Bool(ImGuiMouseButton_Left, false)) {
				dirtyArea = EditorOnMouseDown(&ed, io->MousePos.x, io->MousePos.y);
			}
			if (igIsMouseDragging(ImGuiMouseButton_Left, 0)) {
				dirtyArea = EditorOnMouseMove(&ed, io->MousePos.x, io->MousePos.y);
			}
			if (igIsMouseReleased_Nil(ImGuiMouseButton_Left)) {
				dirtyArea = EditorOnMouseUp(&ed, io->MousePos.x, io->MousePos.y);
			}

			if (dirtyArea.min_x < totalDirty.min_x) totalDirty.min_x = dirtyArea.min_x;
			if (dirtyArea.min_y < totalDirty.min_y) totalDirty.min_y = dirtyArea.min_y;
			if (dirtyArea.max_x > totalDirty.max_x) totalDirty.max_x = dirtyArea.max_x;
			if (dirtyArea.max_y > totalDirty.max_y) totalDirty.max_y = dirtyArea.max_y;

			if (igIsMouseReleased_Nil(ImGuiMouseButton_Left) && totalDirty.max_x > 0) {
				totalDirty = (mmRect_t){ ed.canvas.image.width, ed.canvas.image.height, 0, 0 };
			}

			// Width & Height are set if change occurs
			if (dirtyArea.max_x > 0) {
				TextureUpdate(ed.canvas.texture, ed.canvas.image.width, ed.canvas.image.height, (unsigned char*)ed.canvas.image.pixels);
				dirtyArea.max_x = 0;
			}
		}

		WindowEndFrame();
	}

	EditorDestroy(&ed);
	return 0;
}

static void _MainWinProcessInput(editor_t* ed) {
	ImGuiIO* io = igGetIO();

	if (!igIsMouseDown_Nil(ImGuiMouseButton_Left)) {
		if (io->MouseWheel > 0) ed->view.scale += 0.15f;
		if (io->MouseWheel < 0) ed->view.scale -= 0.15f;
		EditorUpdateView(ed);
	}

	if (igIsKeyPressed_Bool(ImGuiKey_Equal, false)) {
		if (io->KeyCtrl) { ed->view.scale += 0.15f;EditorUpdateView(ed); }
		else ed->tool.brush.size += 1;
	} else if (igIsKeyPressed_Bool(ImGuiKey_Minus, false)) {
		if (io->KeyCtrl) { ed->view.scale -= 0.15f; EditorUpdateView(ed); }
		else if (ed->tool.brush.size > 1) ed->tool.brush.size -= 1;
	} else if (igIsKeyPressed_Bool(ImGuiKey_B, false)) {
		ed->tool.type.current = TOOL_BRUSH;
		ed->tool.brush.rounded = io->KeyShift ? false : true;
	} else if (igIsKeyPressed_Bool(ImGuiKey_E, false)) {
		ed->tool.type.current = TOOL_ERASER;
		ed->tool.brush.rounded = io->KeyShift ? false : true;
	} else if (igIsKeyPressed_Bool(ImGuiKey_Space, false)) {
		ed->tool.type.previous = ed->tool.type.current;
		ed->tool.type.current = TOOL_PAN;
	} else if (igIsKeyReleased_Nil(ImGuiKey_Space)) {
		ed->tool.type.current = ed->tool.type.previous;
	}
}

void AppDestroy(void) {
	WindowDestroy();
}

#if defined(TARGET_WINDOWS)
	#include <windows.h>
	#include <shellapi.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void AppOpenURL(const char* url) {
#if defined(TARGET_WINDOWS)
	ShellExecute(0, 0, url, 0, 0, SW_SHOW);
#elif defined(TARGET_APPLE) || defined(TARGET_LINUX)
	unsigned int len = strlen(url) + 100;
	char* cmd = malloc(len);
	int ret = snprintf(
	    cmd, len,
		#ifdef TARGET_APPLE
			"open \"%s\"",
		#else
		    "setsid xdg-open \"%s\"",
		#endif
	    url
	);
	if (ret > 0 && ret < len) {
		system(cmd);
	}
	free(cmd);
#else
	#error "AppOpenUrl(...) Not Implemented For Target"
#endif
}
