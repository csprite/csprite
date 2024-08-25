#include <stdlib.h>
#include <string.h>

#include "app/app.h"
#include "app/window.h"
#include "cimgui.h"
#include "cimgui_impl.h"
#include "assets/assets.h"
#include "fs/fs.h"
#include "image/image.h"
#include "log/log.h"

#include "sfd.h"
#include "app/editor.h"

void _AppOpenFile(editor_t* ed) {
	const char* filePath = sfd_open_dialog(&(sfd_Options){
		.title        = "Open Image File",
		.filter_name  = "Image File",
		.filter       = "*.png|*.jpg",
		.save         = 0
	});

	if (filePath) {
		editor_t new = {0};
		if (!EditorInitFrom(&new, filePath)) {
			EditorDestroy(ed);
			*ed = new;
			EditorCenterView(ed, (Vec2_t){ igGetIO()->DisplaySize.x, igGetIO()->DisplaySize.y });
		}
	} else {
		const char* LastError = sfd_get_error();
		if (LastError != NULL) {
			log_error("Failed to launch dialog: %s", LastError);
		}
	}
}

void _AppSaveFile(editor_t* ed) {
	if (ed->file.path == NULL) {
		const char* filePath = sfd_open_dialog(&(sfd_Options){
			.title       = "Save Image File",
			.filter_name = "Image File",
			.filter      = "*.png|*.jpg",
			.save        = 1
		});
		if (filePath == NULL) {
			const char* LastError = sfd_get_error();
			if (LastError != NULL) {
				log_error("Failed to launch dialog: %s", LastError);
			}
			return;
		} else {
			int baseName = FsGetBasename(filePath);
			if (baseName < 0) {
				log_error("Failed to find basename of '%s'", filePath);
				return;
			}

			if (ImageWriteTo(&ed->canvas.image, filePath)) {
				return;
			}

			int len = strlen(filePath) + 1;
			ed->file.path = (char*)malloc(len);
			strncpy(ed->file.path, filePath, len);
			ed->file.name = &ed->file.path[baseName];
		}
	} else {
		ImageWriteTo(&ed->canvas.image, ed->file.path);
	}
}

int AppMainLoop(void) {
	WindowNewFrame();
	WindowEndFrame();

	ImGuiIO* io = igGetIO();

	editor_t ed = {0};
	EditorInit(&ed, 120, 90);
	ed.view.scale = 5;
	EditorUpdateView(&ed);
	EditorCenterView(&ed, (Vec2_t){ io->DisplaySize.x, io->DisplaySize.y });

	bool doOpenNewFileModal = false;

	while (!WindowShouldClose()) {
		WindowNewFrame();

		ImVec2 mBarPos;
		ImVec2 mBarSize;
		igBeginMainMenuBar();
			if (igBeginMenu("File", true)) {
				if (igMenuItem_Bool("New", NULL, false, true)) {
					doOpenNewFileModal = true;
				}
				if (igMenuItem_Bool("Open", NULL, false, true)) {
					_AppOpenFile(&ed);
				}
				if (igMenuItem_Bool("Save", NULL, false, true)) {
					_AppSaveFile(&ed);
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
			igText("%s", ed.file.name ? ed.file.name : "untitled");

			igGetWindowPos(&mBarPos);
			igGetWindowSize(&mBarSize);
		igEndMainMenuBar();

		static ImVec2 SidebarPos, SidebarSize;
		SidebarSize.y = io->DisplaySize.y - (mBarPos.y + mBarSize.y); // Used as Constraint & To Reduce Duplicate Calculations
		igSetNextWindowPos((ImVec2){ 0, mBarPos.y + mBarSize.y }, ImGuiCond_Always, (ImVec2){ 0, 0 });
		igSetNextWindowSize((ImVec2){ 200, 0 }, ImGuiCond_Once);
		igSetNextWindowSizeConstraints((ImVec2){ 100, SidebarSize.y }, (ImVec2){ io->DisplaySize.x / 3, SidebarSize.y }, NULL, NULL);
		igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0);
		if (igBegin("##Sidebar", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar)) {
			float color_float[4] = {
				ed.tool.brush.color.r / 255.0f,
				ed.tool.brush.color.g / 255.0f,
				ed.tool.brush.color.b / 255.0f,
				ed.tool.brush.color.a / 255.0f
			};
			ImVec2 availReg;
			igGetContentRegionAvail(&availReg);
			igSetNextItemWidth(availReg.x);
			if (igColorPicker4("##ColorPicker", (float*)&color_float, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview, NULL)) {
				ed.tool.brush.color.r = color_float[0] * 255;
				ed.tool.brush.color.g = color_float[1] * 255;
				ed.tool.brush.color.b = color_float[2] * 255;
				ed.tool.brush.color.a = color_float[3] * 255;
			}
			if (igIsItemHovered(0)) {
				igSetMouseCursor(ImGuiMouseCursor_Hand);
			}

			igGetWindowPos(&SidebarPos);
			igGetWindowSize(&SidebarSize);
			igEnd();
		}
		igPopStyleVar(1);

		ImVec2 statusBarPos, statusBarSize;
		igSetNextWindowPos((ImVec2){ SidebarPos.x + SidebarSize.x, mBarPos.y + mBarSize.y }, ImGuiCond_Always, (ImVec2){ 0, 0 });
		igSetNextWindowSize((ImVec2){ io->DisplaySize.x, 0 }, ImGuiCond_Always);
		if (igBegin("##StatusBar", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImVec2 tWidth;
			igCalcTextSize(&tWidth, "BRUSH_XXX", NULL, false, -1);
			igSetNextItemWidth(tWidth.x);
			if (igBeginCombo("##ToolSelector", ToolToString(ed.tool.type.current), 0)) {
				for (int i = 0; i < TOOL_NONE; i++) {
					if (igSelectable_Bool(ToolToString(i), (tool_t)i == ed.tool.type.current, 0, (ImVec2){0,0})) {
						ed.tool.type.current = i;
					}
				}
				igEndCombo();
			}

			float step = ed.view.scale > 1 ? 0.15 : 0.05;
			igCalcTextSize(&tWidth, "ZOOM_XXXXXXX", NULL, false, -1);
			igSetNextItemWidth(tWidth.x);
			igSameLine(0, -1);
			if (igInputFloat("##ZoomControl", &ed.view.scale, step, step, "x%.2f", 0)) {
				ed.view.scale = ed.view.scale < 0.05 ? 0.05 : ed.view.scale;
				EditorUpdateView(&ed);
			}

			igSameLine(0, -1);
			igText("x: %d, y: %d", (int)((io->MousePos.x - ed.view.x)/ed.view.scale), (int)((io->MousePos.y - ed.view.y)/ed.view.scale));

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

		if (doOpenNewFileModal) {
			doOpenNewFileModal = false;
			igOpenPopup_Str("##NewFileModal", 0);
		}
		if (igBeginPopupModal("##NewFileModal", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			static int width = 20, height = 20;
			if (igInputInt("Width", &width, 1, 5, 0)) width = width < 2 ? 2 : width;
			if (igInputInt("Height", &height, 1, 5, 0)) height = height < 2 ? 2 : height;

			if (igButton("Create", (ImVec2){0,0})) {
				editor_t new = {0};
				if (!EditorInit(&new, width, height)) {
					EditorDestroy(&ed);
					ed = new;
					ed.view.x = (io->DisplaySize.x / 2) - (ed.view.w / 2);
					ed.view.y = (io->DisplaySize.y / 2) - (ed.view.h / 2);
					EditorUpdateView(&ed);
				}

				igCloseCurrentPopup();
				width = 20;
				height = 20;
			}
			igSameLine(0, -1);
			if (igButton("Cancel", (ImVec2){0,0})) {
				igCloseCurrentPopup();
				width = 20;
				height = 20;
			}

			igEndPopup();
		}

		if (isMainWindowHovered) {
			igSetMouseCursor(ImGuiMouseCursor_None);
			EditorProcessInput(&ed);
		}

		WindowEndFrame();
	}

	EditorDestroy(&ed);
	return 0;
}

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
	long long len = strlen(url) + 100;
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
