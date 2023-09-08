#include <iostream>
#include <limits>

// For Converting Strings To LowerCase in FixFileExtension function
#include <algorithm>
#include <cctype>

#include "imgui/imgui.h"

#include "main.hpp"
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
#include "fs/fs.hpp"

int main() {
	if (ImBase::Window::Init(700, 500, "csprite") != 0) {
		return 1;
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	{
		int uiFontSize = 0;
		const void* uiFont = assets_get("data/fonts/NotoSans-Regular.ttf", &uiFontSize);
		if (uiFont) {
			constexpr float fontSizePx = 18.0f;
			ImVector<ImWchar> ranges;
			ImFontGlyphRangesBuilder builder;
			builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
			builder.BuildRanges(&ranges);

			io.Fonts->AddFontFromMemoryCompressedTTF(uiFont, uiFontSize, fontSizePx, nullptr, ranges.Data);
			io.Fonts->Build();
			if (!io.Fonts->IsBuilt()) {
				printf("Failed to build the font!");
			}
		} else {
			printf("Error: uiFont is NULL!\n");
		}
	}

	printf("Config Dir: %s\n",   Fs::GetConfigDir().c_str()); // ~/.config/csprite
	printf("Parent Dir: %s\n",   Fs::GetParentDir(Fs::GetConfigDir()).c_str()); // ~/.config
	printf("Base Name: %s\n",    Fs::GetBaseName(Fs::GetConfigDir()).c_str()); // csprite
	printf("Language Dir: %s\n", Fs::GetLanguageDir().c_str()); // ~/.config/csprite/languages

	i32 fSize = Fs::GetFileSize("./Makefile");
	if (fSize < 0) {
		printf("File Size (./Makefile): %s\n", strerror(errno));
	} else {
		printf("File Size (./Makefile): %d bytes\n", fSize);
	}

	ImBase::Window::NewFrame();
	ImBase::Window::EndFrame();

	// Mouse Position On Window
	ImVec2 MousePos; // mouse position
	ImVec2 MousePosLast; // mouse position last frame

	ImVec2 MousePosRel; // mouse position relative to canvas
	ImVec2 MousePosRelLast; // mouse position relative to canvas last frame

	DocumentState dState;
	String ZoomText = "Zoom: " + std::to_string(dState.ZoomLevel) + "x";

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

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;

	int NEW_DIMS[2] = {60, 40}; // Default Width, Height New Canvas if Created One

	ToolType LastToolType = ToolManager::GetToolType();
	ToolShape LastToolShape = ToolManager::GetToolShape();
	Pixel EmptyColor = { 0, 0, 0, 0 };

	ZoomNCenterVP(dState.ZoomLevel, *dState.doc);

	bool ShowNewCanvasWindow = false; // Holds Whether to show new canvas window or not.
	bool ShowOpenFileWindow = false;
	bool ShowAboutWindow = false;
	imgui_addons::ImGuiFileBrowser FileDialog;

	while (!ImBase::Window::ShouldClose()) {
		ImBase::Window::NewFrame();

		bool isCanvasHovered = io.MousePos.x > dState.doc->viewport.x &&
						io.MousePos.y > dState.doc->viewport.y &&
						io.MousePos.x < dState.doc->viewport.x + dState.doc->viewport.w &&
						io.MousePos.y < dState.doc->viewport.y + dState.doc->viewport.h;

#ifdef _DEBUG
		static bool metricsWinVisible = false;
#endif

		#define BEGIN_MENU(label) if (ImGui::BeginMenu(label)) {
		#define END_MENU() ImGui::EndMenu(); }

		#define BEGIN_MENUITEM(label, shortcut) if (ImGui::MenuItem(label, shortcut)) {
		#define END_MENUITEM() }

		static ImVec2 MenuBarPos;
		static ImVec2 MenuBarSize;

		if (ImGui::BeginMainMenuBar()) {
			BEGIN_MENU("File")
				BEGIN_MENUITEM("New", "Ctrl+N")
					ShowNewCanvasWindow = true;
				END_MENUITEM()
				BEGIN_MENUITEM("Open", "Ctrl+O")
					ShowOpenFileWindow = true;
				END_MENUITEM()
			END_MENU()

#ifdef _DEBUG
			BEGIN_MENU("Dev")
				BEGIN_MENUITEM("Metrics", NULL) metricsWinVisible = !metricsWinVisible; END_MENUITEM()
			END_MENU()
#endif

			BEGIN_MENU("Help")
				BEGIN_MENUITEM("About", NULL)
					ShowAboutWindow = true;
				END_MENUITEM()
				BEGIN_MENUITEM("GitHub", NULL)
					ImBase::Launcher::OpenUrl("https://github.com/pegvin/CSprite");
				END_MENUITEM()
			END_MENU()

			MenuBarPos = ImGui::GetWindowPos();
			MenuBarSize = ImGui::GetWindowSize();
			ImGui::EndMainMenuBar();
		}

		#undef BEGIN_MENUITEM
		#undef END_MENUITEM
		#undef BEGIN_MENU
		#undef END_MENU

		#define BEGIN_WINDOW(label, isOpenPtr, flags) if (ImGui::Begin(label, isOpenPtr, flags)) {
		#define END_WINDOW() ImGui::End(); }

		isCanvasHovered = isCanvasHovered && !(ImGui::IsPopupOpen("Open File###OpenFileWindow") || ImGui::IsPopupOpen("New Document###NewCanvasWindow") || ImGui::IsPopupOpen("About###AboutCspriteWindow"));
		if (ShowOpenFileWindow) {
			ShowOpenFileWindow = false;
			ImGui::OpenPopup("Open File###OpenFileWindow");
		} else if (ShowNewCanvasWindow) {
			ShowNewCanvasWindow = false;
			ImGui::OpenPopup("New Document###NewCanvasWindow");
		} else if (ShowAboutWindow) {
			ShowAboutWindow = false;
			ImGui::OpenPopup("About###AboutCspriteWindow");
		}

		if (FileDialog.showFileDialog(
			"Open File###OpenFileWindow",
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

		#define BEGIN_POPUP(name, flags) if (ImGui::BeginPopupModal(name, NULL, flags)) { isCanvasHovered = false;
		#define END_POPUP() ImGui::EndPopup(); }

		ImGui::SetNextWindowSize({280, 100}, 0);
		BEGIN_POPUP("New Document###NewCanvasWindow", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)
			ImGui::InputInt("width", &NEW_DIMS[0], 1, 1, 0);
			ImGui::InputInt("height", &NEW_DIMS[1], 1, 1, 0);

			if (ImGui::Button("Ok")) {
				delete dState.doc;
				dState.doc = new Doc();
				dState.doc->CreateNew(NEW_DIMS[0], NEW_DIMS[1]);
				dState.doc->AddLayer("New Layers");
				dirtyArea = { 0, 0, dState.doc->w, dState.doc->h };

				ZoomNCenterVP(dState.ZoomLevel, *dState.doc);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
		END_POPUP()

		BEGIN_POPUP("About###AboutCspriteWindow", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)
			ImGui::SeparatorText("Thanks contributors!");
			ImGui::Text("csprite is awesome because of the awesome people who decided");
			ImGui::Text("to give their precious time to improve the project,");
			ImGui::Text("you can see them by");
			ImGui::SameLine();
			static bool isTextHovered = false;
			ImVec4 TextColor = isTextHovered ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Text];
			ImGui::TextColored(TextColor, "clicking here");
			ImVec2 Min = ImGui::GetItemRectMin();
			ImVec2 Max = ImGui::GetItemRectMax();
			Min.y = Max.y; // move the top left co-ordinate to bottom-left
			ImGui::GetWindowDrawList()->AddLine(Min, Max, ImGui::GetColorU32(TextColor));
			isTextHovered = ImGui::IsItemHovered();
			if (isTextHovered) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			}
			if (ImGui::IsItemClicked()) {
				ImBase::Launcher::OpenUrl("https://github.com/csprite/csprite/graphs/contributors");
			}

			ImGui::SeparatorText("Thanks Open-Source Projects!");
			ImGui::Text("csprite uses few open-source projects made by awesome people.\n\ngithub.com/");
			ImGui::BulletText("csprite/imbase - BSD-3-Clause License:");
			ImGui::Indent();
			ImGui::BulletText("glfw/glfw (OpenGL, window & input) - Zlib License");
			ImGui::BulletText("dav1dde/glad (OpenGL Function Loader) - Public Domain");
			ImGui::BulletText("ocornut/imgui (User Interface) - MIT License");
			ImGui::Unindent();
			ImGui::BulletText("nothings/stb (Image Read/Write) - Public Domain");
			ImGui::BulletText("gallickgunner/ImGui-Addons (File Browser) - MIT License");
		END_POPUP()

		#undef BEGIN_POPUP
		#undef END_POPUP

		// Saves Few CPU & GPU Time Since There's No Window Flags Processing Or Some Other Overhead.
		ImGui::GetBackgroundDrawList()->AddRect(
			{ dState.doc->viewport.x - 1, dState.doc->viewport.y - 1 },
			{ dState.doc->viewport.w + dState.doc->viewport.x + 1, dState.doc->viewport.h + dState.doc->viewport.y + 1 },
			ImGui::GetColorU32(ImGuiCol_Border), 0.0f, 0, 1.0f
		);
		ImGui::GetBackgroundDrawList()->AddImage(
			reinterpret_cast<ImTextureID>(dState.doc->tex->id),
			{ dState.doc->viewport.x, dState.doc->viewport.y },
			{ dState.doc->viewport.w + dState.doc->viewport.x, dState.doc->viewport.h + dState.doc->viewport.y }
		);

#ifdef _DEBUG
		if (metricsWinVisible) {
			ImGui::ShowMetricsWindow(NULL);
		}
#endif

		BEGIN_WINDOW("ToolAndZoomWindow", NULL, window_flags | ImGuiWindowFlags_NoBringToFrontOnFocus |  ImGuiWindowFlags_NoFocusOnAppearing)
			ImGui::SetWindowPos({0, MenuBarPos.y + MenuBarSize.y });
			std::string selectedToolText;

			switch (ToolManager::GetToolType()) {
				case BRUSH:
					if (ToolManager::GetToolShape() == ToolShape::CIRCLE) {
						selectedToolText = "Circle Brush - (Size: " + std::to_string(ToolManager::GetBrushSize()) + ")";
					} else {
						selectedToolText = "Square Brush - (Size: " + std::to_string(ToolManager::GetBrushSize()) + ")";
					}
					break;
				case ERASER:
					if (ToolManager::GetToolShape() == ToolShape::CIRCLE) {
						selectedToolText = "Circle Eraser - (Size: " + std::to_string(ToolManager::GetBrushSize()) + ")";
					} else {
						selectedToolText = "Square Eraser - (Size: " + std::to_string(ToolManager::GetBrushSize()) + ")";
					}
					break;
				case INK_DROPPER:
					selectedToolText = "Ink Dropper";
					break;
				case PAN:
					selectedToolText = "Panning";
					break;
			}

			ImVec2 textSize1 = ImGui::CalcTextSize(selectedToolText.c_str(), NULL, false, -2.0f);
			ImVec2 textSize2 = ImGui::CalcTextSize(ZoomText.c_str(), NULL, false, -2.0f);
			ImGui::SetWindowSize({(float)(textSize1.x + textSize2.x), (float)(textSize1.y + textSize2.y) * 2}); // Make Sure Text is visible everytime.

			ImGui::Text("%s", selectedToolText.c_str());
			ImGui::Text("%s", ZoomText.c_str());
		END_WINDOW()

		BEGIN_WINDOW("ColorPaletteWindow", NULL, window_flags)
			ImGui::SetWindowPos({ 0, io.DisplaySize.y - 35.0f });
			for (auto i = 0UL; i < dState.palette.Colors.size(); i++) {
				if (i != 0) ImGui::SameLine();
				if (ImGui::ColorButton(
					dState.PaletteIndex == i ? "Selected Color" : ("Color##" + std::to_string(i)).c_str(),
					{(float)dState.palette[i].r/255, (float)dState.palette[i].g/255, (float)dState.palette[i].b/255, (float)dState.palette[i].a/255})
				) {
					dState.PaletteIndex = i;
					dState.SelectedColor = dState.palette[dState.PaletteIndex];
				}
				if (dState.PaletteIndex == i)
					ImGui::GetWindowDrawList()->AddRect(
						ImGui::GetItemRectMin(),
						ImGui::GetItemRectMax(),
						IM_COL32_WHITE
					);
			};
		END_WINDOW()

		#undef BEGIN_WINDOW
		#undef END_WINDOW

		if (isCanvasHovered) {
			MousePosLast = MousePos;
			MousePosRelLast = MousePosRel;

			MousePos = ImGui::GetMousePos();
			MousePosRel.x = (MousePos[0] - dState.doc->viewport.x) / dState.ZoomLevel;
			MousePosRel.y = (MousePos[1] - dState.doc->viewport.y) / dState.ZoomLevel;

			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				if (io.MouseWheel > 0) AdjustZoom(true, dState.ZoomLevel, ZoomText, *dState.doc);
				if (io.MouseWheel < 0) AdjustZoom(false, dState.ZoomLevel, ZoomText, *dState.doc);
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Equal, false)) {
				if (io.KeyCtrl) AdjustZoom(true, dState.ZoomLevel, ZoomText, *dState.doc);
				else if (io.KeyShift && !io.KeyCtrl)
					dState.PaletteIndex = dState.PaletteIndex >= dState.palette.Colors.size() - 1 ? 0 : dState.PaletteIndex + 1;
				else ToolManager::SetBrushSize(ToolManager::GetBrushSize() + 1);
			} else if (ImGui::IsKeyPressed(ImGuiKey_Minus, false)) {
				if (io.KeyCtrl) AdjustZoom(false, dState.ZoomLevel, ZoomText, *dState.doc);
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
				if (io.KeyCtrl) ShowNewCanvasWindow = true;
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
	return 0;
}

inline void ZoomNCenterVP(u32 ZoomLevel, Doc& d) {
	d.viewport.x = ImGui::GetIO().DisplaySize.x / 2 - (float)d.w * ZoomLevel / 2;
	d.viewport.y = ImGui::GetIO().DisplaySize.y / 2 - (float)d.h * ZoomLevel / 2;
	d.viewport.w = d.w * ZoomLevel;
	d.viewport.h = d.h * ZoomLevel;
}

void AdjustZoom(bool Increase, u32& ZoomLevel, String& ZoomText, Doc& d) {
	if (Increase == true) {
		if (ZoomLevel < std::numeric_limits<u32>().max()) { // Max Value Of Unsigned int
			ZoomLevel++;
		}
	} else {
		if (ZoomLevel != 1) { // if zoom is 1 then don't decrease it further
			ZoomLevel--;
		}
	}

	// Comment Out To Not Center When Zooming
	d.viewport.x = ImGui::GetIO().DisplaySize.x / 2 - (float)d.w * ZoomLevel / 2;
	d.viewport.y = ImGui::GetIO().DisplaySize.y / 2 - (float)d.h * ZoomLevel / 2;

	d.viewport.w = d.w * ZoomLevel;
	d.viewport.h = d.h * ZoomLevel;
	ZoomText = "Zoom: " + std::to_string(ZoomLevel) + "x";
}
