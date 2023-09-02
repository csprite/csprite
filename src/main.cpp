#include <limits>

// For Converting Strings To LowerCase in FixFileExtension function
#include <algorithm>
#include <cctype>

#include "imgui/imgui.h"

#include "assets.h"
#include "main.hpp"
#include "types.hpp"
#include "imbase/window.hpp"
#include "imbase/launcher.hpp"
#include "tools/tools.hpp"
#include "pixel/pixel.hpp"
#include "palette/palette.hpp"
#include "doc/doc.hpp"

Doc* mainDoc = nullptr;
u16 PaletteIndex = 0;
u16 PaletteCount = 16;

Palette ColorPalette;

unsigned int ZoomLevel = 8; // Default Zoom Level
std::string ZoomText = "Zoom: " + std::to_string(ZoomLevel) + "x"; // Human Readable string decribing zoom level for UI
Pixel SelectedColor; // Holds Pointer To Currently Selected Color

bool ShouldSave = false;
bool ShowNewCanvasWindow = false; // Holds Whether to show new canvas window or not.

// Mouse Position On Window
ImVec2 MousePos; // mouse position
ImVec2 MousePosLast; // mouse position last frame

ImVec2 MousePosRel; // mouse position relative to canvas
ImVec2 MousePosRelLast; // mouse position relative to canvas last frame

int main() {
	if (ImBase::Window::Init(700, 500, "csprite") != 0) {
		return 1;
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImBase::Window::NewFrame();
	ImBase::Window::EndFrame();

	ColorPalette.Add(Pixel{ 0,   0,   0,   255 });
	ColorPalette.Add(Pixel{ 29,  43,  83,  255 });
	ColorPalette.Add(Pixel{ 126, 37,  83,  255 });
	ColorPalette.Add(Pixel{ 0,   135, 81,  255 });
	ColorPalette.Add(Pixel{ 171, 82,  54,  255 });
	ColorPalette.Add(Pixel{ 95,  87,  79,  255 });
	ColorPalette.Add(Pixel{ 194, 195, 199, 255 });
	ColorPalette.Add(Pixel{ 255, 241, 232, 255 });
	ColorPalette.Add(Pixel{ 255, 0,   77,  255 });
	ColorPalette.Add(Pixel{ 255, 163, 0,   255 });
	ColorPalette.Add(Pixel{ 255, 236, 39,  255 });
	ColorPalette.Add(Pixel{ 0,   228, 54,  255 });
	ColorPalette.Add(Pixel{ 41,  173, 255, 255 });
	ColorPalette.Add(Pixel{ 131, 118, 156, 255 });
	ColorPalette.Add(Pixel{ 255, 119, 168, 255 });
	ColorPalette.Add(Pixel{ 255, 204, 170, 255 });
	SelectedColor = ColorPalette[PaletteIndex];

	mainDoc = new Doc();
	mainDoc->CreateNew(60, 40);
	mainDoc->AddLayer("New Layer");

	RectI32 dirtyArea = { 0, 0, mainDoc->w, mainDoc->h };

	// Initial Canvas Position & Size
	mainDoc->viewport.x = io.DisplaySize.x / 2 - (float)mainDoc->w * ZoomLevel / 2;
	mainDoc->viewport.y = io.DisplaySize.y / 2 - (float)mainDoc->h * ZoomLevel / 2;
	mainDoc->viewport.w = mainDoc->w * ZoomLevel;
	mainDoc->viewport.h = mainDoc->h * ZoomLevel;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;

	int NEW_DIMS[2] = {60, 40}; // Default Width, Height New Canvas if Created One

	ToolType LastToolType = ToolManager::GetToolType();
	ToolShape LastToolShape = ToolManager::GetToolShape();
	Pixel EmptyColor = { 0, 0, 0, 0 };

	ZoomNCenterVP();

	while (!ImBase::Window::ShouldClose()) {
		ImBase::Window::NewFrame();

		bool isCanvasHovered = io.MousePos.x > mainDoc->viewport.x &&
						io.MousePos.y > mainDoc->viewport.y &&
						io.MousePos.x < mainDoc->viewport.x + mainDoc->viewport.w &&
						io.MousePos.y < mainDoc->viewport.y + mainDoc->viewport.h;

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
					ShowNewCanvasWindow = 1;
				END_MENUITEM()
			END_MENU()

#ifdef _DEBUG
			BEGIN_MENU("Dev")
				BEGIN_MENUITEM("Metrics", NULL) metricsWinVisible = !metricsWinVisible; END_MENUITEM()
			END_MENU()
#endif

			BEGIN_MENU("Help")
				BEGIN_MENUITEM("About", NULL)
					ImBase::Launcher::OpenUrl("https://github.com/pegvin/CSprite/wiki/About-CSprite");
				END_MENUITEM()
				BEGIN_MENUITEM("GitHub", NULL)
					ImBase::Launcher::OpenUrl("https://github.com/pegvin/CSprite");
				END_MENUITEM()
				// BEGIN_MENUITEM("Credits", NULL)
				// 	ImGui::OpenPopup("Lmao###CreditsPopup");
				// END_MENUITEM()
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

		#define BEGIN_POPUP(name, flags) if (ImGui::BeginPopupModal(name, NULL, flags)) { isCanvasHovered = false;
		#define END_POPUP() ImGui::EndPopup(); }

		if (ShowNewCanvasWindow == 1) {
			ImGui::SetNextWindowSize({280, 100}, 0);
			ImGui::OpenPopup("New Document###NewCanvasWindow");
			BEGIN_POPUP("New Document###NewCanvasWindow", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)
				ImGui::InputInt("width", &NEW_DIMS[0], 1, 1, 0);
				ImGui::InputInt("height", &NEW_DIMS[1], 1, 1, 0);

				if (ImGui::Button("Ok")) {
					delete mainDoc;
					mainDoc = new Doc();
					mainDoc->CreateNew(NEW_DIMS[0], NEW_DIMS[1]);
					mainDoc->AddLayer("New Layers");
					mainDoc->layers[0]->pixels = new Pixel[mainDoc->GetTotalPixels()]{ 0, 0, 0, 0 };
					dirtyArea = { 0, 0, mainDoc->w, mainDoc->h };

					ZoomNCenterVP();
					ShowNewCanvasWindow = 0;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					ShowNewCanvasWindow = 0;
					ImGui::CloseCurrentPopup();
				}
			END_POPUP()
		}

		#undef BEGIN_POPUP
		#undef END_POPUP

		// Saves Few CPU & GPU Time Since There's No Window Flags Processing Or Some Other Overhead.
		ImGui::GetBackgroundDrawList()->AddRect(
			{ mainDoc->viewport.x - 1, mainDoc->viewport.y - 1 },
			{ mainDoc->viewport.w + mainDoc->viewport.x + 1, mainDoc->viewport.h + mainDoc->viewport.y + 1 },
			ImGui::GetColorU32(ImGuiCol_Border), 0.0f, 0, 1.0f
		);
		ImGui::GetBackgroundDrawList()->AddImage(
			reinterpret_cast<ImTextureID>(mainDoc->tex->id),
			{ mainDoc->viewport.x, mainDoc->viewport.y },
			{ mainDoc->viewport.w + mainDoc->viewport.x, mainDoc->viewport.h + mainDoc->viewport.y }
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
			for (int i = 0; i < PaletteCount; i++) {
				if (i != 0) ImGui::SameLine();
				if (ImGui::ColorButton(
					PaletteIndex == i ? "Selected Color" : ("Color##" + std::to_string(i)).c_str(),
					{(float)ColorPalette[i].r/255, (float)ColorPalette[i].g/255, (float)ColorPalette[i].b/255, (float)ColorPalette[i].a/255})
				) {
					PaletteIndex = i;
					SelectedColor = ColorPalette[PaletteIndex];
				}
				if (PaletteIndex == i)
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
			MousePosRel.x = (MousePos[0] - mainDoc->viewport.x) / ZoomLevel;
			MousePosRel.y = (MousePos[1] - mainDoc->viewport.y) / ZoomLevel;

			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				if (io.MouseWheel > 0) AdjustZoom(true);
				if (io.MouseWheel < 0) AdjustZoom(false);
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Equal, false)) {
				if (io.KeyCtrl) AdjustZoom(true);
				else if (io.KeyShift && !io.KeyCtrl)
					PaletteIndex = PaletteIndex >= PaletteCount - 1 ? 0 : PaletteIndex + 1;
				else ToolManager::SetBrushSize(ToolManager::GetBrushSize() + 1);
			} else if (ImGui::IsKeyPressed(ImGuiKey_Minus, false)) {
				if (io.KeyCtrl) AdjustZoom(false);
				else if (io.KeyShift && !io.KeyCtrl)
					PaletteIndex = PaletteIndex > 0 ? PaletteIndex - 1 : PaletteCount - 1;
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
				if (io.KeyCtrl) ShowNewCanvasWindow = 1;
			}

			SelectedColor = ColorPalette[PaletteIndex];

			if (ToolManager::GetToolType() == ToolType::PAN) {
				mainDoc->viewport.x += io.MouseDelta.x;
				mainDoc->viewport.y += io.MouseDelta.y;
			}

			double x, y;
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				x = MousePosRel.x;
				y = MousePosRel.y;

				if (x >= 0 && x < mainDoc->w && y >= 0 && y < mainDoc->h) {
					switch (ToolManager::GetToolType()) {
						case BRUSH:
							ToolManager::Draw(
								x, y, mainDoc->w, mainDoc->h,
								SelectedColor, mainDoc->layers[0]->pixels
							);
							ToolManager::DrawBetween(
								x, y, MousePosRelLast.x, MousePosRelLast.y,
								mainDoc->w, mainDoc->h,
								SelectedColor, mainDoc->layers[0]->pixels
							);
							break;
						case ERASER: {
							ToolManager::Draw(
								x, y, mainDoc->w, mainDoc->h,
								EmptyColor, mainDoc->layers[0]->pixels
							);
							ToolManager::DrawBetween(
								x, y, MousePosRelLast.x, MousePosRelLast.y,
								mainDoc->w, mainDoc->h,
								EmptyColor, mainDoc->layers[0]->pixels
							);
							break;
						}
						case INK_DROPPER: {
							Pixel& color = mainDoc->layers[0]->pixels[(u32)((y * mainDoc->w) + x)];

							// For loop starts from 1 because we don't need the first color i.e. 0,0,0,0 or transparent black
							for (int i = 0; i < PaletteCount; i++) {
								if (ColorPalette[i] == color) {
									PaletteIndex = i;
									break;
								}
							}
							break;
						}
						default: {
							break;
						}
					}
					mainDoc->Render(dirtyArea);
				}
			}
		}

		ImBase::Window::EndFrame();
	}

	delete mainDoc;
	ImBase::Window::Destroy();
	return 0;
}

inline void ZoomNCenterVP() {
	mainDoc->viewport.x = ImGui::GetIO().DisplaySize.x / 2 - (float)mainDoc->w * ZoomLevel / 2;
	mainDoc->viewport.y = ImGui::GetIO().DisplaySize.y / 2 - (float)mainDoc->h * ZoomLevel / 2;
	mainDoc->viewport.w = mainDoc->w * ZoomLevel;
	mainDoc->viewport.h = mainDoc->h * ZoomLevel;
}

void AdjustZoom(bool increase) {
	if (increase == true) {
		if (ZoomLevel < std::numeric_limits<u32>().max()) { // Max Value Of Unsigned int
			ZoomLevel++;
		}
	} else {
		if (ZoomLevel != 1) { // if zoom is 1 then don't decrease it further
			ZoomLevel--;
		}
	}

	// Comment Out To Not Center When Zooming
	mainDoc->viewport.x = ImGui::GetIO().DisplaySize.x / 2 - (float)mainDoc->w * ZoomLevel / 2;
	mainDoc->viewport.y = ImGui::GetIO().DisplaySize.y / 2 - (float)mainDoc->h * ZoomLevel / 2;

	mainDoc->viewport.w = mainDoc->w * ZoomLevel;
	mainDoc->viewport.h = mainDoc->h * ZoomLevel;
	ZoomText = "Zoom: " + std::to_string(ZoomLevel) + "x";
}
