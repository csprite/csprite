#ifndef CSP_TOOLS_TOOLMANAGER_HPP_INCLUDED_
#define CSP_TOOLS_TOOLMANAGER_HPP_INCLUDED_ 1
#pragma once

#include "types.h"
#include "pixel/pixel.h"
#include <climits>

struct Doc;

namespace Tool {
	enum Type {
		NONE = 0,
		BRUSH,
		ERASER,
		PAN,
		COLOR_PICKER
	};

	inline const char* ToolTypeToString(Tool::Type t) {
		switch (t) {
			case BRUSH: return "Brush";
			case ERASER: return "Eraser";
			case PAN: return "Pan";
			case COLOR_PICKER: return "Color picker";
			default: return "Unknown";
		}
	}

	struct Manager {
		u32 brushSize = 6;
		bool isRounded = false; // Brushes & Erasers can be rounded too
		Tool::Type currTool = Tool::Type::BRUSH,
		           prevTool = Tool::Type::BRUSH;

		// Secondary Color in Future? ;p
		Pixel primaryColor;
		u32   activeLayer = 0;

		f32     viewportScale = 5; // Min Zoom 0.15
		RectF32 viewport; // viewport position & scaled size

		// recalculate Viewport position & size after `viewportScale` changes
		void UpdateViewportScale(const Doc& doc);

		mm_RectU32 onMouseDown(i32 x, i32 y, Doc& doc);
		mm_RectU32 onMouseMove(i32 x, i32 y, Doc& doc);
		mm_RectU32 onMouseUp(i32 x, i32 y, Doc& doc);

	private:
		VecI32 MousePosDown = { 0, 0 };
		VecI32 MousePosLast = { 0, 0 };
	};
}

#endif // CSP_TOOLS_TOOLMANAGER_HPP_INCLUDED_
