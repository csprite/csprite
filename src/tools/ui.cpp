#include "tools/ui.hpp"

void Tools_UI_Draw(const Tool::Manager& mgr) {
	switch (mgr.currTool) {
		case Tool::Type::BRUSH:
		case Tool::Type::ERASER: {
			ImGui::Text(
				"%s %s - (Size: %u) | Zoom: %.2f",
				mgr.isRounded ? "Circle" : "Square",
				mgr.currTool == Tool::BRUSH ? "Brush" : "Eraser",
				mgr.brushSize, mgr.viewportScale
			);
			break;
		}
		case Tool::Type::PAN: {
			ImGui::Text("Panning | Zoom: %.2f", mgr.viewportScale);
			break;
		}
		case Tool::Type::COLOR_PICKER: {
			ImGui::Text("Color Picker | Zoom: %.2f", mgr.viewportScale);
			break;
		}
		case Tool::Type::NONE: {
			ImGui::Text("Zoom: %.2f", mgr.viewportScale);
			break;
		}
	}
}
