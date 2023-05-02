#include "log.hpp"
#include "window/gui.hpp"
#include "window/renderer.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer.h"

void Gui::Init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    ImGui::StyleColorsDark();

	if (Window::Get() == NULL || Renderer::Get() == NULL) {
		LOG_E("Failed to create ImGui Context, Window or Renderer Not Initialized!");
	    ImGui::DestroyContext();
		return;
	}

    ImGui_ImplSDL2_InitForSDLRenderer(Window::Get(), Renderer::Get());
    ImGui_ImplSDLRenderer_Init(Renderer::Get());
}

void Gui::ProcessEvents(Window::Event* e) {
    ImGui_ImplSDL2_ProcessEvent(e);
}

void Gui::BuildCmdBuffer() {
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Text("Hello Bitchess!!!");

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

float Gui::GetDisplayFbScaleX() {
    return ImGui::GetIO().DisplayFramebufferScale.x;
}

float Gui::GetDisplayFbScaleY() {
    return ImGui::GetIO().DisplayFramebufferScale.y;
}

void Gui::Destroy() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}
