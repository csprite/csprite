#include "os/gfx.h"
#include "app/gui.h"
#include "app/imgui.h"
#include "app/editor.h"

int main(void) {
	// Initialization
	OS_Handle window = os_window_init(320, 240, str8_lit("csprite"));

	// NOTE(pegvin) - Move Editor GUI Related Functions Into gui_*
	Editor ed = Editor_Init(120, 90);

	gui_init(window);
	os_window_show(window);

	gui_begin_frame(window);
	gui_end_frame(window);

	ImGuiIO* io = igGetIO_Nil();
	ed.view.scale = 5;
	Editor_UpdateView(&ed);
	Editor_CenterView(&ed, (Rect){ io->DisplaySize.x, io->DisplaySize.y });

	bool doOpenNewFileModal = false;

	while (!os_window_should_close(window)) {
		gui_begin_frame(window);

		ImVec2 mBarPos;
		ImVec2 mBarSize;
		igBeginMainMenuBar();
			if (igBeginMenu("File", true)) {
				if (igMenuItem_Bool("New", NULL, false, true)) {
					doOpenNewFileModal = true;
				}
				// if (igMenuItem_Bool("Open", NULL, false, true)) {
				// 	_app_open_file(&ed);
				// }
				// if (igMenuItem_Bool("Save", NULL, false, true)) {
				// 	_app_save_file(&ed);
				// }
				igEndMenu();
			}
			if (igBeginMenu("Help", true)) {
				if (igMenuItem_Bool("About", NULL, false, true)) {
					os_open_in_browser(str8_lit("https://csprite.github.io"));
				}
				igEndMenu();
			}

			ImVec2 winSize, textSize;
			igGetWindowSize(&winSize);
			igCalcTextSize(&textSize, ed.file.name, NULL, false, -1);
			igSetCursorPosX((winSize.x - textSize.x) * 0.5);
			igTextUnformatted(ed.file.name ? ed.file.name : "untitled", NULL);

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
				for (S32 i = 0; i < TOOL_NONE; i++) {
					if (igSelectable_Bool(ToolToString(i), (Tool)i == ed.tool.type.current, 0, (ImVec2){0,0})) {
						ed.tool.type.current = i;
					}
				}
				igEndCombo();
			}

			igSameLine(0, -1);
			if (igInputInt("##BrushSize", (S32*)&ed.tool.brush.size, 1, 1, 0)) {
				if (ed.tool.brush.size < 1) {
					ed.tool.brush.size = 1;
				}
			}

			igSameLine(0, -1);
			static bool checked = 0;
			if (igCheckbox("Filled", &checked)) {
				ed.tool.brush.filled = checked;
			}

			float step = ed.view.scale > 1 ? 0.15 : 0.05;
			igCalcTextSize(&tWidth, "ZOOM_XXXXXXX", NULL, false, -1);
			igSetNextItemWidth(tWidth.x);
			igSameLine(0, -1);
			if (igInputFloat("##ZoomControl", &ed.view.scale, step, step, "x%.2f", 0)) {
				ed.view.scale = ed.view.scale < 0.05 ? 0.05 : ed.view.scale;
				Editor_UpdateView(&ed);
			}

			igSameLine(0, -1);
			igText("x: %d, y: %d", (S32)((io->MousePos.x - ed.view.x)/ed.view.scale), (S32)((io->MousePos.y - ed.view.y)/ed.view.scale));

			igGetWindowPos(&statusBarPos);
			igGetWindowSize(&statusBarSize);
			igEnd();
		}

		igSetNextWindowPos((ImVec2){ SidebarPos.x + SidebarSize.x, statusBarPos.y + statusBarSize.y }, ImGuiCond_Always, (ImVec2){ 0, 0 });
		igSetNextWindowSize((ImVec2){ io->DisplaySize.x, io->DisplaySize.y - (statusBarPos.y + statusBarSize.y) + 1 }, ImGuiCond_Always);
		if (igBegin("Main", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
			ImDrawList_AddRect(
			    igGetWindowDrawList(),
			    (ImVec2){ ed.view.x, ed.view.y }, (ImVec2){ ed.view.x + ed.view.w, ed.view.y + ed.view.h },
			    igGetColorU32_Col(ImGuiCol_Border, 1), 0, 0, 1
			);
			ImDrawList_AddImage(
			    igGetWindowDrawList(), r_tex_to_ImTextureID(ed.canvas.checker),
			    (ImVec2){ ed.view.x, ed.view.y }, (ImVec2){ ed.view.x + ed.view.w, ed.view.y + ed.view.h },
			    (ImVec2){ 0, 0 }, (ImVec2){ 1, 1 }, 0xFFFFFFFF
			);
			ImDrawList_AddImage(
			    igGetWindowDrawList(), r_tex_to_ImTextureID(ed.canvas.texture),
			    (ImVec2){ ed.view.x, ed.view.y }, (ImVec2){ ed.view.x + ed.view.w, ed.view.y + ed.view.h },
			    (ImVec2){ 0, 0 }, (ImVec2){ 1, 1 }, 0xFFFFFFFF
			);

			if (igIsWindowHovered(0)) {
				igSetMouseCursor(ImGuiMouseCursor_None);
				Editor_ProcessInput(&ed);
			}

			igEnd();
		}

		if (doOpenNewFileModal) {
			doOpenNewFileModal = false;
			igOpenPopup_Str("##NewFileModal", 0);
		}
		if (igBeginPopupModal("##NewFileModal", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			static S32 width = 20, height = 20;
			if (igInputInt("Width", &width, 1, 5, 0)) width = width < 2 ? 2 : width;
			if (igInputInt("Height", &height, 1, 5, 0)) height = height < 2 ? 2 : height;

			if (igButton("Create", (ImVec2){0,0})) {
				Editor_Deinit(&ed);
				ed = Editor_Init(width, height);
				ed.view.x = (io->DisplaySize.x / 2) - (ed.view.w / 2);
				ed.view.y = (io->DisplaySize.y / 2) - (ed.view.h / 2);
				Editor_UpdateView(&ed);
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

		gui_end_frame(window);
	}

	Editor_Deinit(&ed);

	// Clean Up
	gui_release(window);
	os_window_release(window);
	return 0;
}
