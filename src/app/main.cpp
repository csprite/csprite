#include <limits>

#include "imgui/imgui.h"

#include "main.hpp"
#include "misc.hpp"
#include "types.hpp"
#include "assets/assets.h"
#include "imbase/window.hpp"
#include "imbase/launcher.hpp"
#include "tools/tools.hpp"
#include "pixel/pixel.hpp"
#include "palette/palette.hpp"
#include "doc/doc.hpp"
#include "doc/parser/parser.hpp"
#include "filebrowser/filebrowser.hpp"
#include "i18n/strings.hpp"
#include "fs/fs.hpp"
#include "assets/manager.hpp"
#include "log/log.h"
#include "config.hpp"

int main() {
	EnableVT100();

	if (!Assets::EnsureFileSystem()) {
		return 1;
	}

	Cfg::Load();
	Cfg::Config& Conf = Cfg::Get();

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

	// Mouse Position On Window
	ImVec2 MousePos; // mouse position
	ImVec2 MousePosLast; // mouse position last frame

	ImVec2 MousePosRel; // mouse position relative to canvas
	ImVec2 MousePosRelLast; // mouse position relative to canvas last frame

	DocumentState dState;

	dState.palette.Add(Pixel{ 0,   0,   0,   255 });
	dState.palette.Add(Pixel{ 29,  43,  83,  255 });
	dState.palette.Add(Pixel{ 126, 37,  83,  255 });
	dState.palette.Add(Pixel{ 0,   135, 81,  255 });
	dState.palette.Add(Pixel{ 171, 82,  54,  255 });
	dState.palette.Add(Pixel{ 95,  87,  79,  255 });
	dState.palette.Add(Pixel{ 194, 195, 199, 255 });
	dState.palette.Add(Pixel{ 255, 241, 232, 255 });
	dState.palette.Add(Pixel{ 255, 0,   77,  255 });
	dState.palette.Add(Pixel{ 255, 163, 0,   255 });
	dState.palette.Add(Pixel{ 255, 236, 39,  255 });
	dState.palette.Add(Pixel{ 0,   228, 54,  255 });
	dState.palette.Add(Pixel{ 41,  173, 255, 255 });
	dState.palette.Add(Pixel{ 131, 118, 156, 255 });
	dState.palette.Add(Pixel{ 255, 119, 168, 255 });
	dState.palette.Add(Pixel{ 255, 204, 170, 255 });
	dState.SelectedColor = dState.palette[dState.PaletteIndex];

	dState.doc = new Doc();
	dState.doc->CreateNew(60, 40);
	dState.doc->AddLayer("New Layer");
	RectI32 dirtyArea = { 0, 0, dState.doc->w, dState.doc->h };

	// Initial Canvas Position & Size
	dState.doc->viewport.x = io.DisplaySize.x / 2 - (float)dState.doc->w * dState.ZoomLevel / 2;
	dState.doc->viewport.y = io.DisplaySize.y / 2 - (float)dState.doc->h * dState.ZoomLevel / 2;
	dState.doc->viewport.w = dState.doc->w * dState.ZoomLevel;
	dState.doc->viewport.h = dState.doc->h * dState.ZoomLevel;

	int NEW_DIMS[2] = {60, 40}; // Default Width, Height New Canvas if Created One

	ToolType LastToolType = ToolManager::GetToolType();
	ToolShape LastToolShape = ToolManager::GetToolShape();
	Pixel EmptyColor = { 0, 0, 0, 0 };

	dState.doc->Render(dirtyArea);
	ZoomNCenterVP(dState.ZoomLevel, *dState.doc);

	imgui_addons::ImGuiFileBrowser FileDialog;

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

			BEGIN_MENU("Language")
				UIString::ListAll([&](const char* fileName) {
					BEGIN_MENUITEM(fileName, NULL)
						Conf.langFileName = fileName;
					END_MENUITEM()
				});
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
			ImVec2(700, 310), ".png,.jpg,.jpeg,.bmp,.psd,.tga"
		)) {
			Doc* d = Parser::ParseImageFile(FileDialog.selected_path.c_str());
			if (d != nullptr) {
				delete dState.doc;
				dState.doc = d;
				dirtyArea = { 0, 0, dState.doc->w, dState.doc->h };
				ZoomNCenterVP(dState.ZoomLevel, *dState.doc);
				dState.doc->Render(dirtyArea);
			}
		}

		#define BEGIN_POPUP(name, flags) if (ImGui::BeginPopupModal(name, NULL, flags)) {
		#define END_POPUP() ImGui::EndPopup(); }

		BEGIN_POPUP(Lang[UISTR::Popup_NewDocument], ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)
			ImGui::InputInt(Lang[UISTR::Popup_NewDocument_WidthInput], &NEW_DIMS[0], 1, 1, 0);
			ImGui::InputInt(Lang[UISTR::Popup_NewDocument_HeightInput], &NEW_DIMS[1], 1, 1, 0);

			if (ImGui::Button(Lang[UISTR::Popup_NewDocument_OkButton])) {
				delete dState.doc;
				dState.doc = new Doc();
				dState.doc->CreateNew(NEW_DIMS[0], NEW_DIMS[1]);
				dState.doc->AddLayer("New Layers");
				dirtyArea = { 0, 0, dState.doc->w, dState.doc->h };

				ZoomNCenterVP(dState.ZoomLevel, *dState.doc);
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
		END_POPUP()

		#undef BEGIN_POPUP
		#undef END_POPUP

#ifdef _DEBUG
		if (ShowMetricsWindow) {
			ImGui::ShowMetricsWindow(NULL);
		}
#endif

		static ImVec2 LeftWinPos, LeftWinSize;
		ImGui::SetNextWindowSizeConstraints({ 40, -1 }, { io.DisplaySize.x / 3, -1 });
		ImGui::SetNextWindowPos({ 0, mBarPos.y + mBarSize.y });
		ImGui::SetNextWindowSize({ 200, io.DisplaySize.y - (mBarPos.y + mBarSize.y)}, ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		BEGIN_WINDOW("Color Palette", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)
			for (auto i = 0UL; i < dState.palette.Colors.size(); i++) {
				ImGui::PushID(&dState.palette[i]);

				if (ImGui::ColorButton(dState.PaletteIndex == i ? "Selected Color" : "Color", dState.palette[i])) {
					dState.PaletteIndex = i;
					dState.SelectedColor = dState.palette[dState.PaletteIndex];
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
					#define NEGATIVE_OFFSET 1.0f
					ImGui::GetWindowDrawList()->AddTriangleFilled(
						ImVec2(
							rMax.x - NEGATIVE_OFFSET,
							rMin.y + (rSz.y / 2.1f) - NEGATIVE_OFFSET
						),
						ImVec2(
							rMin.x + (rSz.x / 2.1f) - NEGATIVE_OFFSET,
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
			LeftWinPos = ImGui::GetWindowPos();
			LeftWinSize = ImGui::GetWindowSize();
		END_WINDOW()
		ImGui::PopStyleVar();

		static ImVec2 StatusWinPos, StatusWinSize;
		ImGui::SetNextWindowPos({ LeftWinPos.x + LeftWinSize.x, LeftWinPos.y });
		ImGui::SetNextWindowSize({ io.DisplaySize.x - (LeftWinPos.x + LeftWinSize.x), 0 });
		BEGIN_WINDOW("StatusBarWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)
			switch (ToolManager::GetToolType()) {
				case BRUSH:
					if (ToolManager::GetToolShape() == ToolShape::CIRCLE) {
						ImGui::Text("Circle Brush - (Size: %u) | Zoom: %ux", ToolManager::GetBrushSize(), dState.ZoomLevel);
					} else {
						ImGui::Text("Square Brush - (Size: %u) | Zoom: %ux", ToolManager::GetBrushSize(), dState.ZoomLevel);
					}
					break;
				case ERASER:
					if (ToolManager::GetToolShape() == ToolShape::CIRCLE) {
						ImGui::Text("Circle Eraser - (Size: %u) | Zoom: %ux", ToolManager::GetBrushSize(), dState.ZoomLevel);
					} else {
						ImGui::Text("Square Eraser - (Size: %u) | Zoom: %ux", ToolManager::GetBrushSize(), dState.ZoomLevel);
					}
					break;
				case INK_DROPPER:
					ImGui::Text("Ink Dropper | Zoom: %ux", dState.ZoomLevel);
					break;
				case PAN:
					ImGui::Text("Panning | Zoom: %ux", dState.ZoomLevel);
					break;
			}
			StatusWinPos = ImGui::GetWindowPos();
			StatusWinSize = ImGui::GetWindowSize();
		END_WINDOW()

		static bool isMainWindowHovered = false;
		ImGui::SetNextWindowPos({ LeftWinPos.x + LeftWinSize.x, StatusWinPos.y + StatusWinSize.y - 1 });
		ImGui::SetNextWindowSize({ StatusWinSize.x, io.DisplaySize.y - (StatusWinPos.y + StatusWinSize.y) + 1 });
		BEGIN_WINDOW("MainWindow", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)
			ImGui::GetWindowDrawList()->AddRect(
				{ dState.doc->viewport.x - 1, dState.doc->viewport.y - 1 },
				{ dState.doc->viewport.w + dState.doc->viewport.x + 1, dState.doc->viewport.h + dState.doc->viewport.y + 1 },
				ImGui::GetColorU32(ImGuiCol_Border), 0.0f, 0, 1.0f
			);
			ImGui::GetWindowDrawList()->AddImage(
				reinterpret_cast<ImTextureID>(dState.doc->tex->id),
				{ dState.doc->viewport.x, dState.doc->viewport.y },
				{ dState.doc->viewport.w + dState.doc->viewport.x, dState.doc->viewport.h + dState.doc->viewport.y }
			);
			isMainWindowHovered = ImGui::IsWindowHovered();
		END_WINDOW()

		#undef BEGIN_WINDOW
		#undef END_WINDOW

		if (isMainWindowHovered) {
			MousePosLast = MousePos;
			MousePosRelLast = MousePosRel;

			MousePos = ImGui::GetMousePos();
			MousePosRel.x = (MousePos[0] - dState.doc->viewport.x) / dState.ZoomLevel;
			MousePosRel.y = (MousePos[1] - dState.doc->viewport.y) / dState.ZoomLevel;

			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				if (io.MouseWheel > 0) AdjustZoom(true, dState.ZoomLevel, *dState.doc);
				if (io.MouseWheel < 0) AdjustZoom(false, dState.ZoomLevel, *dState.doc);
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Equal, false)) {
				if (io.KeyCtrl) AdjustZoom(true, dState.ZoomLevel, *dState.doc);
				else if (io.KeyShift && !io.KeyCtrl)
					dState.PaletteIndex = dState.PaletteIndex >= dState.palette.Colors.size() - 1 ? 0 : dState.PaletteIndex + 1;
				else ToolManager::SetBrushSize(ToolManager::GetBrushSize() + 1);
			} else if (ImGui::IsKeyPressed(ImGuiKey_Minus, false)) {
				if (io.KeyCtrl) AdjustZoom(false, dState.ZoomLevel, *dState.doc);
				else if (io.KeyShift && !io.KeyCtrl)
					dState.PaletteIndex = dState.PaletteIndex > 0 ? dState.PaletteIndex - 1 : dState.palette.Colors.size() - 1;
				else if (ToolManager::GetBrushSize() > 2)
					ToolManager::SetBrushSize(ToolManager::GetBrushSize() - 1);
			} else if (ImGui::IsKeyPressed(ImGuiKey_B, false)) {
				ToolManager::SetToolType(ToolType::BRUSH);
				ToolManager::SetToolShape(io.KeyShift ? ToolShape::SQUARE : ToolShape::CIRCLE);
			} else if (ImGui::IsKeyPressed(ImGuiKey_E, false)) {
				ToolManager::SetToolType(ToolType::ERASER);
				ToolManager::SetToolShape(io.KeyShift ? ToolShape::SQUARE : ToolShape::CIRCLE);
			} else if (ImGui::IsKeyPressed(ImGuiKey_I, false)) {
				LastToolType = ToolManager::GetToolType();
				LastToolShape = ToolManager::GetToolShape();
				ToolManager::SetToolType(ToolType::INK_DROPPER);
			} else if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
				LastToolType = ToolManager::GetToolType();
				LastToolShape = ToolManager::GetToolShape();
				ToolManager::SetToolType(ToolType::PAN);
			} else if (ImGui::IsKeyReleased(ImGuiKey_Space)) {
				ToolManager::SetToolType(LastToolType);
				ToolManager::SetToolShape(LastToolShape);
			} else if (ImGui::IsKeyPressed(ImGuiKey_N, false)) {
				if (io.KeyCtrl) ShowNewDocumentWindow = true;
			}

			dState.SelectedColor = dState.palette[dState.PaletteIndex];

			if (ToolManager::GetToolType() == ToolType::PAN) {
				dState.doc->viewport.x += io.MouseDelta.x;
				dState.doc->viewport.y += io.MouseDelta.y;
			}

			double x, y;
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				x = MousePosRel.x;
				y = MousePosRel.y;

				if (x >= 0 && x < dState.doc->w && y >= 0 && y < dState.doc->h) {
					switch (ToolManager::GetToolType()) {
						case BRUSH:
							ToolManager::Draw(
								x, y, dState.doc->w, dState.doc->h,
								dState.SelectedColor, dState.doc->layers[0]->pixels
							);
							ToolManager::DrawBetween(
								x, y, MousePosRelLast.x, MousePosRelLast.y,
								dState.doc->w, dState.doc->h,
								dState.SelectedColor, dState.doc->layers[0]->pixels
							);
							break;
						case ERASER: {
							ToolManager::Draw(
								x, y, dState.doc->w, dState.doc->h,
								EmptyColor, dState.doc->layers[0]->pixels
							);
							ToolManager::DrawBetween(
								x, y, MousePosRelLast.x, MousePosRelLast.y,
								dState.doc->w, dState.doc->h,
								EmptyColor, dState.doc->layers[0]->pixels
							);
							break;
						}
						case INK_DROPPER: {
							Pixel& color = dState.doc->layers[0]->pixels[(u32)((y * dState.doc->w) + x)];

							// For loop starts from 1 because we don't need the first color i.e. 0,0,0,0 or transparent black
							for (auto i = 0UL; i < dState.palette.Colors.size(); i++) {
								if (dState.palette[i] == color) {
									dState.PaletteIndex = i;
									break;
								}
							}
							break;
						}
						default: {
							break;
						}
					}
					dState.doc->Render(dirtyArea);
				}
			}
		}

		ImBase::Window::EndFrame();
	}

	delete dState.doc;
	ImBase::Window::Destroy();
	Cfg::Write();
	return 0;
}

inline void ZoomNCenterVP(u32 ZoomLevel, Doc& d) {
	d.viewport.x = ImGui::GetIO().DisplaySize.x / 2 - (float)d.w * ZoomLevel / 2;
	d.viewport.y = ImGui::GetIO().DisplaySize.y / 2 - (float)d.h * ZoomLevel / 2;
	d.viewport.w = d.w * ZoomLevel;
	d.viewport.h = d.h * ZoomLevel;
}

void AdjustZoom(bool Increase, u32& ZoomLevel, Doc& d) {
	if (Increase == true) {
		if (ZoomLevel < std::numeric_limits<u32>().max()) { // Max Value Of Unsigned int
			ZoomLevel++;
		}
	} else {
		if (ZoomLevel != 1) { // if zoom is 1 then don't decrease it further
			ZoomLevel--;
		}
	}

	// This Ensures That The Canvas Is Zoomed From It's Center And Not From The Bottom Left Position
	RectI32 CurrRectCenter;
	CurrRectCenter.x = (d.viewport.w / 2) + d.viewport.x;
	CurrRectCenter.y = (d.viewport.h / 2) + d.viewport.y;

	RectI32 NewRectCenter;
	NewRectCenter.x = (d.w * ZoomLevel / 2) + (i32)d.viewport.x;
	NewRectCenter.y = (d.h * ZoomLevel / 2) + (i32)d.viewport.y;

	d.viewport.x -= NewRectCenter.x - CurrRectCenter.x;
	d.viewport.y -= NewRectCenter.y - CurrRectCenter.y;

	d.viewport.w = d.w * ZoomLevel;
	d.viewport.h = d.h * ZoomLevel;
}

