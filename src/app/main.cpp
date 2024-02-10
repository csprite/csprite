#include <limits>

#include "imgui/imgui.h"

#include "main.hpp"
#include "misc.hpp"
#include "tools/ToolManager.hpp"
#include "types.hpp"
#include "assets/assets.h"
#include "imbase/window.hpp"
#include "imbase/launcher.hpp"
#include "pixel/pixel.hpp"
#include "palette/palette.hpp"
#include "doc/doc.hpp"
#include "image/parser.hpp"
#include "filebrowser/filebrowser.hpp"
#include "i18n/strings.hpp"
#include "assets/manager.hpp"
#include "log/log.h"
#include "config.hpp"

#include "fswrapper.hpp"
#include "palette/parser.hpp"

int main() {
	EnableVT100();

	if (!Assets::EnsureFileSystem()) {
		return 1;
	}

	Cfg::Load();
	Cfg::Config& Conf = Cfg::Get();

	PaletteHelper::UpdateEntries();
	UIString::UpdateEntries();
	if (!UIString::LoadFile(Conf.langFileName)) {
		UIString::LoadDefault();
	}
	const UISTR_Arr& Lang = UIString::Get();

	if (ImBase::Window::Init(700, 500, "csprite") != 0) {
		return 1;
	}
	ImBase::Window::SetMaxFPS(Conf.fps);

	ImGuiIO& io = ImGui::GetIO();

	int uiFontSzBytes = 0;
	ImVector<ImWchar> FontRanges;
	ImFontGlyphRangesBuilder FontBuilder;
	FontBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());
	FontBuilder.AddRanges(UIString::GetRanges());
	FontBuilder.BuildRanges(&FontRanges);
	io.Fonts->AddFontFromMemoryCompressedTTF(
		assets_get("data/fonts/NotoSans-Regular.ttf", &uiFontSzBytes),
		uiFontSzBytes, 18.0f, nullptr, FontRanges.Data
	);
	io.Fonts->Build();
	if (!io.Fonts->IsBuilt()) {
		log_error("io.Fonts->Build() - failed to build the font atlas");
	}

	ImBase::Window::NewFrame();
	ImBase::Window::EndFrame();

	DocumentState dState;
	PaletteHelper::LoadDefault(dState.palette);
	dState.tManager.primaryColor = dState.palette[dState.PaletteIndex];

	dState.doc.Create(60, 40);
	dState.doc.image.AddLayer("New Layer");
	RectU32 dirtyArea = { 0, 0, dState.doc.image.w, dState.doc.image.h };

	// Initial Canvas Position & Size
	dState.tManager.viewport.x = io.DisplaySize.x / 2 - (float)dState.doc.image.w * dState.tManager.viewportScale / 2;
	dState.tManager.viewport.y = io.DisplaySize.y / 2 - (float)dState.doc.image.h * dState.tManager.viewportScale / 2;
	dState.tManager.viewport.w = dState.doc.image.w * dState.tManager.viewportScale;
	dState.tManager.viewport.h = dState.doc.image.h * dState.tManager.viewportScale;

	int NEW_DIMS[2] = {60, 40}; // Default Width, Height New Canvas if Created One

	dState.doc.Render(dirtyArea);
	ZoomNCenterVP(dState.tManager, dState.doc);

	imgui_addons::ImGuiFileBrowser FileDialog;

	ImVec2 MousePosRel;
	bool ShowNewDocumentWindow = false;
	bool ShowOpenFileWindow = false;
	bool ShowAboutWindow = false;
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
					ShowNewDocumentWindow = true;
				END_MENUITEM()
				BEGIN_MENUITEM(Lang[UISTR::MenuItem_Open], "Ctrl+O")
					ShowOpenFileWindow = true;
				END_MENUITEM()
			END_MENU()

			BEGIN_MENU("Edit")
				BEGIN_MENU("Palette")
					BEGIN_MENUITEM("Refresh", NULL)
						PaletteHelper::UpdateEntries();
					END_MENUITEM()
					PaletteHelper::ListAll([&](const char* fileName) {
						BEGIN_MENUITEM(fileName, NULL)
							const String filePath = FileSystem::GetPalettesDir() + PATH_SEP_CHAR + fileName;
							Palette pal;
							if (PaletteParser::Parse(pal, filePath)) {
								dState.palette = std::move(pal);
								dState.PaletteIndex = 0;
								dState.tManager.primaryColor = dState.palette[dState.PaletteIndex];
							}
						END_MENUITEM()
					});
				END_MENU()
				BEGIN_MENU("Language")
					UIString::ListAll([&](const char* fileName) {
						BEGIN_MENUITEM(fileName, NULL)
							Conf.langFileName = fileName;
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

		if (ShowOpenFileWindow) {
			ShowOpenFileWindow = false;
			ImGui::OpenPopup(Lang[UISTR::Popup_OpenFile]);
		} else if (ShowNewDocumentWindow) {
			ShowNewDocumentWindow = false;
			ImGui::OpenPopup(Lang[UISTR::Popup_NewDocument]);
		} else if (ShowAboutWindow) {
			ShowAboutWindow = false;
			ImGui::OpenPopup(Lang[UISTR::Popup_AboutCsprite]);
		}

		if (FileDialog.showFileDialog(
			Lang[UISTR::Popup_OpenFile],
			imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
			ImVec2(700, 310), IMAGE_SUPPORTED_EXTENSIONS
		)) {
			Image img;
			if (ImageParser::Parse(img, FileDialog.selected_path.c_str())) {
				dState.doc.Destroy();
				dState.doc.Create(img.w, img.h);
				dState.doc.image = std::move(img);
				dirtyArea = { 0, 0, dState.doc.image.w, dState.doc.image.h };
				ZoomNCenterVP(dState.tManager, dState.doc);
				dState.doc.Render(dirtyArea);
			}
		}

		#define BEGIN_POPUP(name, flags) if (ImGui::BeginPopupModal(name, NULL, flags)) {
		#define END_POPUP() ImGui::EndPopup(); }

		BEGIN_POPUP(Lang[UISTR::Popup_NewDocument], ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)
			ImGui::InputInt(Lang[UISTR::Popup_NewDocument_WidthInput], &NEW_DIMS[0], 1, 1, 0);
			ImGui::InputInt(Lang[UISTR::Popup_NewDocument_HeightInput], &NEW_DIMS[1], 1, 1, 0);

			if (ImGui::Button(Lang[UISTR::Popup_NewDocument_OkButton])) {
				dState.doc.Destroy();
				dState.doc.Create(NEW_DIMS[0], NEW_DIMS[1]);
				dState.doc.image.AddLayer("New Layer");
				dirtyArea = { 0, 0, dState.doc.image.w, dState.doc.image.h };

				ZoomNCenterVP(dState.tManager, dState.doc);
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
			ImGui::BulletText("csprite/imbase - BSD-3-Clause License:");
				ImGui::Indent();
				ImGui::BulletText("glfw/glfw (OpenGL, window & input) - Zlib License");
				ImGui::BulletText("dav1dde/glad (OpenGL Function Loader) - Public Domain");
				ImGui::BulletText("ocornut/imgui (User Interface) - MIT License");
			ImGui::Unindent();
			ImGui::BulletText("rxi/log.c (Logging) - MIT License");
			ImGui::BulletText("nothings/stb (Image Read/Write) - Public Domain");
			ImGui::BulletText("gallickgunner/ImGui-Addons (File Browser) - MIT License");
			ImGui::BulletText("nlohmann/json (JSON Parser) - MIT License");
				ImGui::Indent();
				ImGui::BulletText("abseil/abseil-cpp - Apache 2.0 License");
				ImGui::BulletText("UTF-8 Decoder by Bjoern Hoehrmann - MIT License");
				ImGui::BulletText("Grisu2 algorithm by Florian Loitsch - MIT License");
			ImGui::Unindent();

			ImGui::Spacing();
			ImVec2 closeButtonSize(100.f, 0.f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - closeButtonSize.x);
			if (ImGui::Button(Lang[UISTR::Popup_AboutCsprite_CloseButton], closeButtonSize)) {
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
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 2, 2 });
			for (auto i = 0UL; i < dState.palette.Colors.size(); i++) {
				ImGui::PushID(&dState.palette[i]);

				if (ImGui::ColorButton(dState.PaletteIndex == i ? "Selected Color" : "Color", dState.palette[i], ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, { ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f })) {
					dState.PaletteIndex = i;
					dState.tManager.primaryColor = dState.palette[dState.PaletteIndex];
				}

				if (dState.PaletteIndex == i) {
					ImVec2 rSz = ImGui::GetItemRectSize();
					ImVec2 rMin = ImGui::GetItemRectMin();
					ImVec2 rMax = ImGui::GetItemRectMax();

					u8 r = dState.palette.Colors[i].r;
					u8 g = dState.palette.Colors[i].g;
					u8 b = dState.palette.Colors[i].b;

					r = CLAMP_NUM_TO_TYPE((r > 127 ? r - 125 : r + 125), u8);
					g = CLAMP_NUM_TO_TYPE((g > 127 ? g - 125 : g + 125), u8);
					b = CLAMP_NUM_TO_TYPE((b > 127 ? b - 125 : b + 125), u8);

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
					i < dState.palette.Colors.size() - 1 &&
					nextBtnSizeX < (ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x)
				) ImGui::SameLine();

				ImGui::PopID();
			};
			ImGui::PopStyleVar(1); // ImGuiStyleVar_ItemSpacing
			LeftWinPos = ImGui::GetWindowPos();
			LeftWinSize = ImGui::GetWindowSize();
		END_WINDOW()
		ImGui::PopStyleVar();

		static ImVec2 StatusWinPos, StatusWinSize;
		ImGui::SetNextWindowPos({ LeftWinPos.x + LeftWinSize.x, LeftWinPos.y });
		ImGui::SetNextWindowSize({ io.DisplaySize.x - (LeftWinPos.x + LeftWinSize.x), 0 });
		BEGIN_WINDOW("StatusBarWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize)
			switch (dState.tManager.currTool) {
				case Tool::Type::BRUSH:
					if (dState.tManager.isRounded) {
						ImGui::Text("Circle Brush - (Size: %u) | Zoom: %.2f | (%d, %d)", dState.tManager.brushSize, dState.tManager.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
					} else {
						ImGui::Text("Square Brush - (Size: %u) | Zoom: %.2f | (%d, %d)", dState.tManager.brushSize, dState.tManager.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
					}
					break;
				case Tool::Type::ERASER:
					if (dState.tManager.isRounded) {
						ImGui::Text("Circle Eraser - (Size: %u) | Zoom: %.2f | (%d, %d)", dState.tManager.brushSize, dState.tManager.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
					} else {
						ImGui::Text("Square Eraser - (Size: %u) | Zoom: %.2f | (%d, %d)", dState.tManager.brushSize, dState.tManager.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
					}
					break;
				case Tool::Type::PAN:
					ImGui::Text("Panning | Zoom: %.2f | (%d, %d)", dState.tManager.viewportScale, (i32)MousePosRel.x, (i32)MousePosRel.y);
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
				{ dState.tManager.viewport.x - 1, dState.tManager.viewport.y - 1 },
				{ dState.tManager.viewport.w + dState.tManager.viewport.x + 1, dState.tManager.viewport.h + dState.tManager.viewport.y + 1 },
				ImGui::GetColorU32(ImGuiCol_Border), 0.0f, 0, 1.0f
			);
			ImGui::GetWindowDrawList()->AddImage(
				reinterpret_cast<ImTextureID>(dState.doc.renderTex->id),
				{ dState.tManager.viewport.x, dState.tManager.viewport.y },
				{ dState.tManager.viewport.w + dState.tManager.viewport.x, dState.tManager.viewport.h + dState.tManager.viewport.y }
			);

			MousePosRel.x = (i32)((io.MousePos.x - dState.tManager.viewport.x) / dState.tManager.viewportScale);
			MousePosRel.y = (i32)((io.MousePos.y - dState.tManager.viewport.y) / dState.tManager.viewportScale);

			const bool MouseInBounds = MousePosRel.x >= 0 && MousePosRel.y >= 0 && MousePosRel.x < dState.doc.image.w && MousePosRel.y < dState.doc.image.h;

			if (MouseInBounds && isMainWindowHovered) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
				ImVec2 TopLeft = {
					(dState.tManager.viewport.x + ((i32)MousePosRel.x * dState.tManager.viewportScale)),
					(dState.tManager.viewport.y + ((i32)MousePosRel.y * dState.tManager.viewportScale))
				};
				ImVec2 BottomRight = {
					TopLeft.x + dState.tManager.viewportScale,
					TopLeft.y + dState.tManager.viewportScale
				};
				const Pixel& p = dState.doc.image.Layers[0].pixels[(i32)(MousePosRel.y * dState.doc.image.w) + (i32)MousePosRel.x];
				ImU32 Color = (p.r * 0.299 + p.g * 0.587 + p.b * 0.114) > 186 ? 0xFF000000 : 0xFFFFFFFF;
				ImGui::GetWindowDrawList()->AddRect(
					TopLeft, BottomRight,
					Color, 0, 0, 0.1 * dState.tManager.viewportScale
				);
			}
		END_WINDOW()

		#undef BEGIN_WINDOW
		#undef END_WINDOW

		if (isMainWindowHovered) {
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				if (io.MouseWheel > 0) AdjustZoom(true, dState.tManager, dState.doc);
				if (io.MouseWheel < 0) AdjustZoom(false, dState.tManager, dState.doc);
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Equal, false)) {
				if (io.KeyCtrl) AdjustZoom(true, dState.tManager, dState.doc);
				else if (io.KeyShift && !io.KeyCtrl)
					dState.PaletteIndex = dState.PaletteIndex >= dState.palette.Colors.size() - 1 ? 0 : dState.PaletteIndex + 1;
				else dState.tManager.brushSize += 1;
			} else if (ImGui::IsKeyPressed(ImGuiKey_Minus, false)) {
				if (io.KeyCtrl) AdjustZoom(false, dState.tManager, dState.doc);
				else if (io.KeyShift && !io.KeyCtrl)
					dState.PaletteIndex = dState.PaletteIndex > 0 ? dState.PaletteIndex - 1 : dState.palette.Colors.size() - 1;
				else if (dState.tManager.brushSize > 1)
					dState.tManager.brushSize -= 1;
			} else if (ImGui::IsKeyPressed(ImGuiKey_B, false)) {
				dState.tManager.currTool = Tool::Type::BRUSH;
				dState.tManager.isRounded = io.KeyShift ? false : true;
			} else if (ImGui::IsKeyPressed(ImGuiKey_E, false)) {
				dState.tManager.currTool = Tool::Type::ERASER;
				dState.tManager.isRounded = io.KeyShift ? false : true;
			} else if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
				dState.tManager.prevTool = dState.tManager.currTool;
				dState.tManager.currTool = Tool::Type::PAN;
			} else if (ImGui::IsKeyReleased(ImGuiKey_Space)) {
				dState.tManager.currTool = dState.tManager.prevTool;
			} else if (ImGui::IsKeyPressed(ImGuiKey_N, false)) {
				if (io.KeyCtrl) ShowNewDocumentWindow = true;
			}

			dState.tManager.primaryColor = dState.palette[dState.PaletteIndex];

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				dirtyArea = dState.tManager.onMouseDown(io.MousePos.x, io.MousePos.y, dState.doc);
			}
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && io.MouseDelta.x != 0 && io.MouseDelta.y != 0) {
				dirtyArea = dState.tManager.onMouseMove(io.MousePos.x, io.MousePos.y, dState.doc);
			}
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				dirtyArea = dState.tManager.onMouseUp(io.MousePos.x, io.MousePos.y, dState.doc);
			}

			// Width & Height are set if change occurs
			if (dirtyArea.w > 0 && dirtyArea.h > 0) {
				dState.doc.Render(dirtyArea);
				dirtyArea.w = 0;
				dirtyArea.h = 0;
			}
		}

		ImBase::Window::EndFrame();
	}

	dState.doc.Destroy();
	ImBase::Window::Destroy();
	Cfg::Write();
	return 0;
}

inline void ZoomNCenterVP(Tool::Manager& mgr, const Doc& doc) {
	mgr.UpdateViewportScale(doc);

	mgr.viewport.x = (ImGui::GetIO().DisplaySize.x / 2) - (mgr.viewportScale / 2);
	mgr.viewport.y = (ImGui::GetIO().DisplaySize.y / 2) - (mgr.viewportScale / 2);
}

void AdjustZoom(bool Increase, Tool::Manager& mgr, const Doc& doc) {
	if (Increase == true) {
		if (mgr.viewportScale < (std::numeric_limits<f32>().max() - 1)) {
			mgr.viewportScale += 0.5;
		}
	} else {
		if (mgr.viewportScale >= 0.30) {
			mgr.viewportScale -= 0.15;
		}
	}
	mgr.UpdateViewportScale(doc);
}

