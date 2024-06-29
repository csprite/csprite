#include "imbase/window.hpp"
#include "imgui/imgui.h"
#include "log/log.h"

int main(void) {
	if (ImBase::Window::Init(640, 480, "csprite", true)) {
		log_error("Failed to Initialize a Window");
		return 1;
	}

	while (!ImBase::Window::ShouldClose()) {
		ImBase::Window::NewFrame();
		ImGui::Text("Hello, World");
		ImBase::Window::EndFrame();
	}

	ImBase::Window::Destroy();
	return 0;
}

