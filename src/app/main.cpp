#include "imgui/imgui.h"
#include "imgui_stdlib.h"

#include "app/main.hpp"
#include "app/cmd.hpp"
#include "app/misc.hpp"
#include "app/prefs.hpp"
#include "app/fswrapper.hpp"
#include "app/i18n/strings.hpp"

#include "log/log.h"
#include "pixel/pixel.h"
#include "palette/parser.hpp"

#include "assets/assets.h"
#include "assets/manager.hpp"

#include "imbase/window.hpp"
#include "imbase/launcher.hpp"

#include "image/ui.hpp"
#include "palette/ui.hpp"
#include "tools/ui.hpp"

int main() {
	EnableVT100();

	if (!Assets::EnsureFileSystem()) {
		return 1;
	}

	Preferences AppPrefs;
	if (!AppPrefs.Load(FileSystem::GetConfigFile().c_str())) {
		AppPrefs = Preferences();
	}

	PaletteHelper::UpdateEntries();
	UIString::UpdateEntries();
	if (AppPrefs.langFileName.empty() || !UIString::LoadFile(AppPrefs.langFileName)) {
		UIString::LoadDefault();
	}
	const UISTR_Arr& Lang = UIString::Get();

	if (ImBase::Window::Init(700, 500, "csprite") != 0) {
		return 1;
	}
	ImBase::Window::SetMaxFPS(AppPrefs.fps);

	const ImGuiIO& io = ImGui::GetIO();

	int uiFontSzBytes = 0;
	ImVector<ImWchar> FontRanges;
	ImFontGlyphRangesBuilder FontBuilder;
	FontBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());
	FontBuilder.AddRanges(UIString::GetRanges());
	FontBuilder.BuildRanges(&FontRanges);
	io.Fonts->AddFontFromMemoryCompressedTTF(
		assets_get("data/fonts/NotoSansMono.ttf", &uiFontSzBytes),
		uiFontSzBytes, AppPrefs.fontSize, nullptr, FontRanges.Data
	);
	io.Fonts->Build();
	if (!io.Fonts->IsBuilt()) {
		log_error("io.Fonts->Build() - failed to build the font atlas");
	}

	ImBase::Window::NewFrame();
	ImBase::Window::EndFrame();

	Editor ed;
	PaletteHelper::LoadDefault(ed.pal);
	ed.mgr.primaryColor = ed.pal[ed.mgr.primaryColorIdx];

	ed.doc.Create(60, 40);
	ed.doc.image.AddLayer();
	mm_RectU32 dirtyArea = { 0, 0, ed.doc.image.w, ed.doc.image.h };

	// Initial Canvas Position & Size
	ed.mgr.viewport.x = io.DisplaySize.x / 2 - (float)ed.doc.image.w * ed.mgr.viewportScale / 2;
	ed.mgr.viewport.y = io.DisplaySize.y / 2 - (float)ed.doc.image.h * ed.mgr.viewportScale / 2;
	ed.mgr.viewport.w = ed.doc.image.w * ed.mgr.viewportScale;
	ed.mgr.viewport.h = ed.doc.image.h * ed.mgr.viewportScale;

	ed.doc.Render(dirtyArea);
	Cmd::Execute(Cmd::Type::Center_Viewport, &ed.mgr, &ed.doc);

	int NewCanvasRes[2] = {60, 40};

	ImVec2 MousePosRel;
	bool ShowNewDocWindow = false,
		 ShowAboutWindow = false,
	     ShowAppPrefsigWindow = false,
	     ShowLayerPropertiesWindow = false;
#ifdef _DEBUG
	bool ShowMetricsWindow = false;
#endif

	while (!ImBase::Window::ShouldClose()) {
		ImBase::Window::NewFrame();

		#define BEGIN_MENU(label) if (ImGui::BeginMenu(label)) {
		#define END_MENU() ImGui::EndMenu(); }

		#define BEGIN_MENUITEM(label, shortcut) if (ImGui::MenuItem(label, shortcut)) {
		#define END_MENUITEM() }

		static ImVec2 mBarPos;
		static ImVec2 mBarSize;
		if (ImGui::BeginMainMenuBar()) {
			BEGIN_MENU(Lang[UISTR::Menu_File])
				BEGIN_MENUITEM(Lang[UISTR::MenuItem_New], "Ctrl+N")
					ShowNewDocWindow = true;
				END_MENUITEM()
				BEGIN_MENUITEM(Lang[UISTR::MenuItem_Open], "Ctrl+O")
					Cmd::Execute(Cmd::Type::Open_File, &ed.doc, &ed.mgr);
				END_MENUITEM()
				BEGIN_MENUITEM("Save", "Ctrl+S")
					Cmd::Execute(Cmd::Type::Save_File, &ed.doc.image, &ed.filePath);
				END_MENUITEM()
				BEGIN_MENUITEM("Save As", "Alt+S")
					Cmd::Execute(Cmd::Type::SaveAs_File, &ed.doc.image, &ed.filePath);
				END_MENUITEM()
			END_MENU()

			BEGIN_MENU("Edit")
				BEGIN_MENUITEM("Preferences", NULL)
					ShowAppPrefsigWindow = true;
				END_MENUITEM()
				BEGIN_MENU("Palette")
					static bool hasItems = false;
					if (ImGui::Button("Refresh", { hasItems ? -1.f : 0, 0 } /* -1 = Fit To Parent, 0 = Default Height/Leave it upto ImGui to calc */)) {
						PaletteHelper::UpdateEntries();
					}
					PaletteHelper::ListAll([&](const char* fileName) {
						hasItems = true;
						BEGIN_MENUITEM(fileName, NULL)
							const String filePath = FileSystem::GetPalettesDir() + PATH_SEP_CHAR + fileName;
							Palette pal;
							if (PaletteParser::Parse(pal, filePath)) {
								ed.pal = std::move(pal);
								ed.mgr.primaryColorIdx = 0;
								ed.mgr.primaryColor = ed.pal[ed.mgr.primaryColorIdx];
							}
						END_MENUITEM()
					});
				END_MENU()
			END_MENU()

#ifdef _DEBUG
			BEGIN_MENU("Dev")
				BEGIN_MENUITEM("Metrics", NULL)
					ShowMetricsWindow = !ShowMetricsWindow;
				END_MENUITEM()
			END_MENU()
#endif

			BEGIN_MENU(Lang[UISTR::Menu_Help])
				BEGIN_MENUITEM(Lang[UISTR::MenuItem_About], NULL)
					ShowAboutWindow = true;
				END_MENUITEM()
				BEGIN_MENUITEM(Lang[UISTR::MenuItem_GitHub], NULL)
					ImBase::Launcher::OpenUrl("https://github.com/csprite/csprite");
				END_MENUITEM()
			END_MENU()

			mBarPos = ImGui::GetWindowPos();
			mBarSize = ImGui::GetWindowSize();
			ImGui::EndMainMenuBar();
		}

		#undef BEGIN_MENUITEM
		#undef END_MENUITEM
		#undef BEGIN_MENU
		#undef END_MENU

		#define BEGIN_WINDOW(label, isOpenPtr, flags) if (ImGui::Begin(label, isOpenPtr, flags)) {
		#define END_WINDOW() ImGui::End(); }

		if (ShowNewDocWindow) {
			ShowNewDocWindow = false;
			ImGui::OpenPopup(Lang[UISTR::Popup_NewDocument]);
		} else if (ShowAboutWindow) {
			ShowAboutWindow = false;
			ImGui::OpenPopup(Lang[UISTR::Popup_AboutCsprite]);
		} else if (ShowAppPrefsigWindow) {
			ShowAppPrefsigWindow = false;
			ImGui::OpenPopup("Preferences##CspritePref");
		} else if (ShowLayerPropertiesWindow) {
			ShowLayerPropertiesWindow = false;
			ImGui::OpenPopup("Properties##LayerProperties");
		}

		#define BEGIN_POPUP(name, flags) if (ImGui::BeginPopupModal(name, NULL, flags)) {
		#define END_POPUP() ImGui::EndPopup(); }

		BEGIN_POPUP(Lang[UISTR::Popup_NewDocument], ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)
			ImGui::InputInt(Lang[UISTR::Popup_NewDocument_WidthInput], &NewCanvasRes[0], 1, 1, 0);
			ImGui::InputInt(Lang[UISTR::Popup_NewDocument_HeightInput], &NewCanvasRes[1], 1, 1, 0);

			if (ImGui::Button(Lang[UISTR::Popup_NewDocument_OkButton])) {
				Cmd::Execute(Cmd::Type::New_File, &ed.doc, NewCanvasRes[0], NewCanvasRes[1]);
				Cmd::Execute(Cmd::Type::Center_Viewport, &ed.mgr, &ed.doc);

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(Lang[UISTR::Popup_NewDocument_CancelButton])) {
				ImGui::CloseCurrentPopup();
			}
		END_POPUP()

		ImGui::SetNextWindowSize({520, 0});
		BEGIN_POPUP(Lang[UISTR::Popup_AboutCsprite], ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)
			ImGui::SeparatorText(Lang[UISTR::Popup_AboutCsprite_Contrib_Header]);
			ImGui::TextWrapped(Lang[UISTR::Popup_AboutCsprite_Contrib_Paragraph]);

			static bool isTextHovered = false;
			ImVec4 TextColor = isTextHovered ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Text];
			ImGui::TextColored(TextColor, Lang[UISTR::Popup_AboutCsprite_Contrib_Link]);
			ImVec2 Min = ImGui::GetItemRectMin();
			ImVec2 Max = ImGui::GetItemRectMax();
			Min.y = Max.y; // move the top left co-ordinate to bottom-left
			isTextHovered = ImGui::IsItemHovered();
			ImGui::GetWindowDrawList()->AddLine(Min, Max, ImGui::GetColorU32(TextColor));
			if (isTextHovered) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			}
			if (ImGui::IsItemClicked()) {
				ImBase::Launcher::OpenUrl("https://github.com/csprite/csprite/graphs/contributors");
			}
			#define TEXT_BULLET(bullet, text) ImGui::TextWrapped(bullet); ImGui::SameLine(); ImGui::TextWrapped(text);

			ImGui::SeparatorText(Lang[UISTR::Popup_AboutCsprite_OpenSrcProjects_Header]);
			ImGui::TextWrapped(Lang[UISTR::Popup_AboutCsprite_OpenSrcProjects_Text]);
			ImGui::TextWrapped("github.com/");
			ImGui::Indent();
			TEXT_BULLET("-", "csprite/imbase - BSD-3-Clause License:");
				ImGui::Indent();
				TEXT_BULLET("$", "glfw/glfw (OpenGL, window & input) - Zlib License");
				TEXT_BULLET("$", "dav1dde/glad (OpenGL Function Loader) - Public Domain");
				TEXT_BULLET("$", "ocornut/imgui (User Interface) - MIT License");
			ImGui::Unindent();
			TEXT_BULLET("-", "rxi/sfd (File Dialog) - MIT License");
			TEXT_BULLET("-", "brofield/simpleini (INI Read/Write) - MIT License");
			TEXT_BULLET("-", "nothings/stb (Image Read/Write) - Public Domain");
			ImGui::Unindent();

			#undef TEXT_BULLET

			ImGui::Spacing();
			ImVec2 closeButtonSize(100.f, 0.f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - closeButtonSize.x);
			if (ImGui::Button(Lang[UISTR::Popup_AboutCsprite_CloseButton], closeButtonSize)) {
				ImGui::CloseCurrentPopup();
			}
		END_POPUP()

		ImGui::SetNextWindowSize({ 400, 250 });
		BEGIN_POPUP("Preferences##CspritePref", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)
			if (Prefs_UI_Draw(AppPrefs)) {
				ImGui::CloseCurrentPopup();
			}
		END_POPUP()

		BEGIN_POPUP("Properties##LayerProperties", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)
			static bool isFirst = true;
			static Layer layerTemp;
			if (isFirst) {
				layerTemp = ed.doc.image.Layers[ed.mgr.activeLayer];
				isFirst = false;
			}

			ImGui::InputText("Name", &layerTemp.name);
			const i32 p_min = 0, p_max = 255;
			ImGui::DragScalar("Opacity", ImGuiDataType_U8, &layerTemp.opacity, 1.0f, &p_min, &p_max);
			if (ImGui::BeginCombo("Blend Mode", BlendModeToString(layerTemp.blend))) {
				for (size_t i = 0; i < Blend::Count; i++) {
					if (ImGui::Selectable(BlendModeToString((Blend)i), layerTemp.blend == i)) {
						layerTemp.blend = (Blend)i;
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Save")) {
				// https://stackoverflow.com/a/217605/14516016
				// Trim Spaces From Start
				auto isNotSpaceCB = [](unsigned char ch) -> bool {
					return !std::isspace(ch);
				};

				layerTemp.name.erase(
					layerTemp.name.begin(),
					std::find_if(layerTemp.name.begin(), layerTemp.name.end(), isNotSpaceCB)
				);

				// Trim Spaces From End
				layerTemp.name.erase(
					std::find_if(layerTemp.name.rbegin(), layerTemp.name.rend(), isNotSpaceCB).base(),
					layerTemp.name.end()
				);

				if (!layerTemp.name.empty()) {
					Layer& currLayer = ed.doc.image.Layers[ed.mgr.activeLayer];
					bool doReRender = currLayer.blend != layerTemp.blend || currLayer.opacity != layerTemp.opacity;
					currLayer = layerTemp;

					if (doReRender) {
						ed.doc.Render({ 0, 0, ed.doc.image.w, ed.doc.image.h });
					}
				}
				isFirst = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				isFirst = true;
				ImGui::CloseCurrentPopup();
			}
		END_POPUP()

		#undef BEGIN_POPUP
		#undef END_POPUP

#ifdef _DEBUG
		if (ShowMetricsWindow) {
			ImGui::ShowMetricsWindow(NULL);
		}
#endif

		static ImVec2 LeftWinPos, LeftWinSize;
		LeftWinSize.y = io.DisplaySize.y - (mBarPos.y + mBarSize.y); // Used as Constraint & To Reduce Duplicate Calculations
		ImGui::SetNextWindowPos({ 0, mBarPos.y + mBarSize.y });
		ImGui::SetNextWindowSize({ 200, 0 }, ImGuiCond_Once);
		ImGui::SetNextWindowSizeConstraints({ 40, LeftWinSize.y }, { io.DisplaySize.x / 3, LeftWinSize.y });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		BEGIN_WINDOW("Side Bar", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar)
			Palette_UI_Draw(ed.pal, ed.mgr.primaryColorIdx, ed.mgr.primaryColor);

			if (ImageLayers_UI_Draw(ed.doc.image, ed.mgr.activeLayer, ShowLayerPropertiesWindow)) {
				if (ed.doc.image.Layers.size() > 0) {
					ed.doc.Render({ 0, 0, ed.doc.image.w, ed.doc.image.h });
				} else {
					ed.doc.ClearRender();
				}
			}

			LeftWinPos = ImGui::GetWindowPos();
			LeftWinSize = ImGui::GetWindowSize();
		END_WINDOW()
		ImGui::PopStyleVar();

		static ImVec2 StatusWinPos, StatusWinSize;
		ImGui::SetNextWindowPos({ LeftWinPos.x + LeftWinSize.x, LeftWinPos.y });
		ImGui::SetNextWindowSize({ io.DisplaySize.x - (LeftWinPos.x + LeftWinSize.x), 0 });
		BEGIN_WINDOW("StatusBarWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize)
			Tools_UI_Draw(ed.mgr);
			StatusWinPos = ImGui::GetWindowPos();
			StatusWinSize = ImGui::GetWindowSize();
		END_WINDOW()

		static bool isMainWindowHovered = false;

		ImGui::SetNextWindowPos({ LeftWinPos.x + LeftWinSize.x, StatusWinPos.y + StatusWinSize.y - 1 });
		ImGui::SetNextWindowSize({ StatusWinSize.x, io.DisplaySize.y - (StatusWinPos.y + StatusWinSize.y) + 1 });
		BEGIN_WINDOW("MainWindow", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize)
			isMainWindowHovered = ImGui::IsWindowHovered();

			ImGui::GetWindowDrawList()->AddRect(
				{ ed.mgr.viewport.x - 1, ed.mgr.viewport.y - 1 },
				{ ed.mgr.viewport.w + ed.mgr.viewport.x + 1, ed.mgr.viewport.h + ed.mgr.viewport.y + 1 },
				ImGui::GetColorU32(ImGuiCol_Border), 0.0f, 0, 1.0f
			);
			ImGui::GetWindowDrawList()->AddImage(
				reinterpret_cast<ImTextureID>(ed.doc.renderTex->id),
				{ ed.mgr.viewport.x, ed.mgr.viewport.y },
				{ ed.mgr.viewport.w + ed.mgr.viewport.x, ed.mgr.viewport.h + ed.mgr.viewport.y }
			);

			MousePosRel.x = (i32)((io.MousePos.x - ed.mgr.viewport.x) / ed.mgr.viewportScale);
			MousePosRel.y = (i32)((io.MousePos.y - ed.mgr.viewport.y) / ed.mgr.viewportScale);

			const bool MouseInBounds = MousePosRel.x >= 0 && MousePosRel.y >= 0 && MousePosRel.x < ed.doc.image.w && MousePosRel.y < ed.doc.image.h;

			if (MouseInBounds && isMainWindowHovered && ed.doc.image.Layers.size() > 0) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
				ImVec2 TopLeft = {
					(ed.mgr.viewport.x + ((i32)MousePosRel.x * ed.mgr.viewportScale)),
					(ed.mgr.viewport.y + ((i32)MousePosRel.y * ed.mgr.viewportScale))
				};
				ImVec2 BottomRight = {
					TopLeft.x + ed.mgr.viewportScale,
					TopLeft.y + ed.mgr.viewportScale
				};
				const Pixel& p = ed.doc.image.Layers[ed.mgr.activeLayer].pixels[(i32)(MousePosRel.y * ed.doc.image.w) + (i32)MousePosRel.x];
				ImU32 Color = (p.r * 0.299 + p.g * 0.587 + p.b * 0.114) > 186 ? 0xFF000000 : 0xFFFFFFFF;
				ImGui::GetWindowDrawList()->AddRect(
					TopLeft, BottomRight,
					Color, 0, 0, 0.1 * ed.mgr.viewportScale
				);
			}
		END_WINDOW()

		#undef BEGIN_WINDOW
		#undef END_WINDOW

		if (isMainWindowHovered) {
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				if (io.MouseWheel > 0) Cmd::Execute(Cmd::Type::ZoomIn_Viewport, &ed.mgr, &ed.doc);
				if (io.MouseWheel < 0) Cmd::Execute(Cmd::Type::ZoomOut_Viewport, &ed.mgr, &ed.doc);
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Equal, false)) {
				if (io.KeyCtrl) Cmd::Execute(Cmd::Type::ZoomIn_Viewport, &ed.mgr, &ed.doc);
				else if (io.KeyShift && !io.KeyCtrl)
					ed.mgr.primaryColorIdx = ed.mgr.primaryColorIdx >= ed.pal.Colors.size() - 1 ? 0 : ed.mgr.primaryColorIdx + 1;
				else ed.mgr.brushSize += 1;
			} else if (ImGui::IsKeyPressed(ImGuiKey_Minus, false)) {
				if (io.KeyCtrl) Cmd::Execute(Cmd::Type::ZoomOut_Viewport, &ed.mgr, &ed.doc);
				else if (io.KeyShift && !io.KeyCtrl)
					ed.mgr.primaryColorIdx = ed.mgr.primaryColorIdx > 0 ? ed.mgr.primaryColorIdx - 1 : ed.pal.Colors.size() - 1;
				else if (ed.mgr.brushSize > 1)
					ed.mgr.brushSize -= 1;
			} else if (ImGui::IsKeyPressed(ImGuiKey_B, false)) {
				ed.mgr.currTool = Tool::Type::BRUSH;
				ed.mgr.isRounded = io.KeyShift ? false : true;
			} else if (ImGui::IsKeyPressed(ImGuiKey_E, false)) {
				ed.mgr.currTool = Tool::Type::ERASER;
				ed.mgr.isRounded = io.KeyShift ? false : true;
			} else if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
				ed.mgr.prevTool = ed.mgr.currTool;
				ed.mgr.currTool = Tool::Type::PAN;
			} else if (ImGui::IsKeyReleased(ImGuiKey_Space)) {
				ed.mgr.currTool = ed.mgr.prevTool;
			} else if (ImGui::IsKeyPressed(ImGuiKey_N, false)) {
				if (io.KeyCtrl) ShowNewDocWindow = true;
			} else if (ImGui::IsKeyPressed(ImGuiKey_I, false)) {
				ed.mgr.currTool = Tool::Type::COLOR_PICKER;
			}

			if (ed.doc.image.Layers.size() > 0) {
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					dirtyArea = ed.mgr.onMouseDown(io.MousePos.x, io.MousePos.y, ed.doc);
				}
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0)) {
					dirtyArea = ed.mgr.onMouseDrag(io.MousePos.x, io.MousePos.y, ed.doc);
				}
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
					dirtyArea = ed.mgr.onMouseUp(io.MousePos.x, io.MousePos.y, ed.doc);
				}

				// Width & Height are set if change occurs
				if (dirtyArea.max_x > 0) {
					ed.doc.Render(dirtyArea);
					dirtyArea.max_x = 0;
				}
			}
		}

		ImBase::Window::EndFrame();
	}

	ed.doc.Destroy();
	ImBase::Window::Destroy();
	return 0;
}
