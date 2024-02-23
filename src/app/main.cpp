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
		assets_get("data/fonts/NotoSans-Regular.ttf", &uiFontSzBytes),
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

			ImGui::SeparatorText(Lang[UISTR::Popup_AboutCsprite_OpenSrcProjects_Header]);
			ImGui::TextWrapped(Lang[UISTR::Popup_AboutCsprite_OpenSrcProjects_Text]);
			ImGui::Text("github.com/");
			ImGui::BulletText("csprite/imbase - BSD-3-Clause License:");
				ImGui::Indent();
				ImGui::BulletText("glfw/glfw (OpenGL, window & input) - Zlib License");
				ImGui::BulletText("dav1dde/glad (OpenGL Function Loader) - Public Domain");
				ImGui::BulletText("ocornut/imgui (User Interface) - MIT License");
			ImGui::Unindent();
			ImGui::BulletText("rxi/sfd (File Dialog) - MIT License");
			ImGui::BulletText("brofield/simpleini (INI Read/Write) - MIT License");
			ImGui::BulletText("nothings/stb (Image Read/Write) - Public Domain");

			ImGui::Spacing();
			ImVec2 closeButtonSize(100.f, 0.f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - closeButtonSize.x);
			if (ImGui::Button(Lang[UISTR::Popup_AboutCsprite_CloseButton], closeButtonSize)) {
				ImGui::CloseCurrentPopup();
			}
		END_POPUP()

		ImGui::SetNextWindowSize({ 400, 250 });
		BEGIN_POPUP("Preferences##CspritePref", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)
			static Preferences tempPrefs = AppPrefs;
			static int32_t CurrentSelection = 0;

			ImGui::BeginTable("##PreferencesTable", 2, ImGuiTableFlags_BordersInnerV);
			ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthFixed, 110.0f, 0);
			ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);

			// set the row height to maximum available content height in the window
			auto vMin_y = ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y;
			auto vMax_y = ImGui::GetWindowContentRegionMax().y + ImGui::GetWindowPos().y;
			ImGui::TableNextRow(ImGuiTableRowFlags_None, (vMax_y - vMin_y) - ImGui::GetFrameHeightWithSpacing());

			ImGui::TableNextColumn();

			if (ImGui::Selectable("General", CurrentSelection == 0, ImGuiSelectableFlags_DontClosePopups)) {
				CurrentSelection = 0;
			} else if (ImGui::Selectable("Languages", CurrentSelection == 1, ImGuiSelectableFlags_DontClosePopups)) {
				CurrentSelection = 1;
			}

			ImGui::TableNextColumn();

			switch (CurrentSelection) {
				case 0: {
					u32 p_step = 1;
					u32 p_stepFast = 5;
					ImGui::InputScalar("Max FPS", ImGuiDataType_U32, &tempPrefs.fps, &p_step, &p_stepFast);
					tempPrefs.fps = tempPrefs.fps < 5 ? 5 : tempPrefs.fps;

					ImGui::InputScalar("Font Size", ImGuiDataType_U32, &tempPrefs.fontSize, &p_step, &p_stepFast);
					tempPrefs.fontSize = tempPrefs.fontSize < 10 ? 10 : tempPrefs.fontSize;
					break;
				}
				case 1: {
					if (ImGui::BeginCombo("##Language", tempPrefs.langFileName.c_str())) {
						UIString::ListAll([&](const char* fileName) {
							if (ImGui::Selectable(fileName)) {
								tempPrefs.langFileName = fileName;
							}
						});
						if (ImGui::Selectable("default")) {
							tempPrefs.langFileName.clear();
						}
						ImGui::EndCombo();
					}
					ImGui::SameLine(0, 3);
					if (ImGui::Button("Refresh")) {
						UIString::UpdateEntries();
					}
					break;
				}
				default: {
					ImGui::TextWrapped("Not-Reachable Section: %d, Please Report This To The Developer", CurrentSelection);
					break;
				}
			}

			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			if (ImGui::Button("Save")) {
				AppPrefs = tempPrefs;
				AppPrefs.Write(FileSystem::GetConfigFile().c_str());
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
				ImGui::SetTooltip("Restart app to apply these changes");
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				tempPrefs = AppPrefs;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndTable();
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
				layerTemp.name.erase(
					layerTemp.name.begin(),
					std::find_if(layerTemp.name.begin(), layerTemp.name.end(), [](unsigned char ch) {
						return !std::isspace(ch);
					})
				);

				// Trim Spaces From End
				layerTemp.name.erase(
					std::find_if(
						layerTemp.name.rbegin(), layerTemp.name.rend(), [](unsigned char ch) {
							return !std::isspace(ch);
						}
					).base(),
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
		BEGIN_WINDOW("Color Palette", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar)
			ImGui::SeparatorText("Colors");

			bool isPrimaryInPalette = false;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 2, 2 });
			for (auto i = 0UL; i < ed.pal.Colors.size(); i++) {
				ImGui::PushID(&ed.pal[i]);

				if (ImGui::ColorButton(ed.mgr.primaryColorIdx == i ? "Selected Color" : "Color", ed.pal[i], ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, { ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f })) {
					ed.mgr.primaryColorIdx = i;
					ed.mgr.primaryColor = ed.pal[ed.mgr.primaryColorIdx];
				}

				if (ed.mgr.primaryColorIdx == i && ed.mgr.primaryColor == ed.pal[i]) {
					isPrimaryInPalette = true;
					ImVec2 rSz = ImGui::GetItemRectSize();
					ImVec2 rMin = ImGui::GetItemRectMin();
					ImVec2 rMax = ImGui::GetItemRectMax();

					u8 r = ed.pal.Colors[i].r;
					u8 g = ed.pal.Colors[i].g;
					u8 b = ed.pal.Colors[i].b;

					r = MIN_MAX((r > 127 ? r - 125 : r + 125), 0, 255);
					g = MIN_MAX((g > 127 ? g - 125 : g + 125), 0, 255);
					b = MIN_MAX((b > 127 ? b - 125 : b + 125), 0, 255);

					/* This Value Will Be Subtracted From Triangle's Positions
					   Because Of Some Extra "Marginal" Space The Button Takes */
					#define NEGATIVE_OFFSET 0.5f
					ImGui::GetWindowDrawList()->AddTriangleFilled(
						ImVec2(
							rMax.x - NEGATIVE_OFFSET,
							rMin.y + (rSz.y / 2.5f) - NEGATIVE_OFFSET
						),
						ImVec2(
							rMin.x + (rSz.x / 2.5f) - NEGATIVE_OFFSET,
							rMax.y - NEGATIVE_OFFSET
						),
						ImVec2(
							rMax.x - NEGATIVE_OFFSET,
							rMax.y - NEGATIVE_OFFSET
						),
						IM_COL32(r, g, b, 200)
					);
					ImGui::GetWindowDrawList()->AddRect(rMin, rMax, IM_COL32(r, g, b, 200));
					#undef NEGATIVE_OFFSET
				}

				// Expected position if next button was on same line
				float nextBtnSizeX = ImGui::GetItemRectMax().x + ImGui::GetItemRectSize().x + ImGui::GetStyle().ItemSpacing.x;
				if (
					i < ed.pal.Colors.size() - 1 &&
					nextBtnSizeX < (ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x)
				) ImGui::SameLine();

				ImGui::PopID();
			};
			ImGui::PopStyleVar(1); // ImGuiStyleVar_ItemSpacing

			if (isPrimaryInPalette) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_Button]);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyle().Colors[ImGuiCol_Button]);
			}
			if (ImGui::Button("Add") && !isPrimaryInPalette) {
				ed.pal.Add(ed.mgr.primaryColor);
				ed.mgr.primaryColorIdx = ed.pal.Colors.size() - 1;
				ed.mgr.primaryColor = ed.pal[ed.mgr.primaryColorIdx];
			}
			if (isPrimaryInPalette) { ImGui::PopStyleVar(); ImGui::PopStyleColor(2); }

			ImGui::SameLine();

			if (!isPrimaryInPalette) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_Button]);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyle().Colors[ImGuiCol_Button]);
			}
			if (ImGui::Button("Remove") && isPrimaryInPalette) {
				ed.pal.Remove(ed.mgr.primaryColor);
				if (ed.mgr.primaryColorIdx > 0) ed.mgr.primaryColorIdx--;
				ed.mgr.primaryColor = ed.pal[ed.mgr.primaryColorIdx];
			}
			if (!isPrimaryInPalette) { ImGui::PopStyleVar(); ImGui::PopStyleColor(2); }

			float ColorPicker[4] = {
				((float)ed.mgr.primaryColor.r) / 255,
				((float)ed.mgr.primaryColor.g) / 255,
				((float)ed.mgr.primaryColor.b) / 255,
				((float)ed.mgr.primaryColor.a) / 255
			};
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::ColorPicker4("##ColorPicker", (float*)&ColorPicker, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview)) {
				ed.mgr.primaryColor.r = ColorPicker[0] * 255;
				ed.mgr.primaryColor.g = ColorPicker[1] * 255;
				ed.mgr.primaryColor.b = ColorPicker[2] * 255;
				ed.mgr.primaryColor.a = ColorPicker[3] * 255;
			}

			ImGui::SeparatorText("Layers");

			if (ImGui::Button("+")) {
				ed.doc.image.AddLayer();
				ed.mgr.activeLayer = ed.doc.image.Layers.size() - 1;
				ed.doc.Render({ 0, 0, ed.doc.image.w, ed.doc.image.h });
			}
			ImGui::SameLine();
			if (ImGui::Button("-") && ed.doc.image.Layers.size() > 0) {
				ed.doc.image.RemoveLayer(ed.mgr.activeLayer);
				ed.mgr.activeLayer = ed.doc.image.Layers.size() - 1;
				if (ed.doc.image.Layers.size() > 0) {
					ed.doc.Render({ 0, 0, ed.doc.image.w, ed.doc.image.h });
				} else {
					ed.doc.ClearRender();
				}
			}

			ImGui::BeginChild("##LayersList", { 0, 0 }, true);

			for (size_t i = 0; i < ed.doc.image.Layers.size(); i++) {
				const Layer& layer = ed.doc.image.Layers[i];
				ImGui::PushID(i);
				if (ImGui::Selectable(layer.name.c_str(), i == ed.mgr.activeLayer, ImGuiSelectableFlags_AllowDoubleClick)) {
					ed.mgr.activeLayer = i;

					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
						ShowLayerPropertiesWindow = true;
					}
				}
				ImGui::PopID();
			}

			ImGui::EndChild();

			LeftWinPos = ImGui::GetWindowPos();
			LeftWinSize = ImGui::GetWindowSize();
		END_WINDOW()
		ImGui::PopStyleVar();

		static ImVec2 StatusWinPos, StatusWinSize;
		ImGui::SetNextWindowPos({ LeftWinPos.x + LeftWinSize.x, LeftWinPos.y });
		ImGui::SetNextWindowSize({ io.DisplaySize.x - (LeftWinPos.x + LeftWinSize.x), 0 });
		BEGIN_WINDOW("StatusBarWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize)
			switch (ed.mgr.currTool) {
				case Tool::Type::BRUSH:
					if (ed.mgr.isRounded) {
						ImGui::Text("Circle Brush - (Size: %u) | Zoom: %.2f | (%d, %d)", ed.mgr.brushSize, ed.mgr.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
					} else {
						ImGui::Text("Square Brush - (Size: %u) | Zoom: %.2f | (%d, %d)", ed.mgr.brushSize, ed.mgr.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
					}
					break;
				case Tool::Type::ERASER:
					if (ed.mgr.isRounded) {
						ImGui::Text("Circle Eraser - (Size: %u) | Zoom: %.2f | (%d, %d)", ed.mgr.brushSize, ed.mgr.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
					} else {
						ImGui::Text("Square Eraser - (Size: %u) | Zoom: %.2f | (%d, %d)", ed.mgr.brushSize, ed.mgr.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
					}
					break;
				case Tool::Type::PAN:
					ImGui::Text("Panning | Zoom: %.2f | (%d, %d)", ed.mgr.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
					break;
				default: break;
			}
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
