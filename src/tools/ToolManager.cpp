#include "types.h"
#include "tools/tools.hpp"
#include "tools/ToolManager.hpp"
#include "doc/doc.hpp"

using namespace Tool;

mm_RectU32 Manager::onMouseDown(i32 x, i32 y, Doc& doc) {
    MousePosDown = { x, y };
	MousePosLast = { x, y };
	mm_RectU32 dirty = { doc.image.w, doc.image.h, 0, 0 };

	VecI32 MousePosRel = {
		(i32)((x - viewport.x) / viewportScale),
		(i32)((y - viewport.y) / viewportScale)
	};

	switch (currTool) {
		case BRUSH:
		case ERASER: {
			dirty = Tool::Draw(
				MousePosRel.x, MousePosRel.y, doc.image.w, doc.image.h,
				isRounded, brushSize, currTool == BRUSH ? primaryColor : Pixel{ 0, 0, 0, 0 },
				doc.image.Layers[activeLayer].pixels
			);
			break;
		}
		case COLOR_PICKER: {
			for (auto it = doc.image.Layers.rbegin(); it != doc.image.Layers.rend(); ++it) {
				Pixel color = it->pixels[(MousePosRel.y * doc.image.w) + MousePosRel.x];
				if (color.a > 0) {
					primaryColor = color;
					break;
				}
			}
			break;
		}
		case NONE:
		case PAN: {
			break;
		}
	}

	return dirty;
}

mm_RectU32 Manager::onMouseMove(i32 x, i32 y, Doc& doc) {
	mm_RectU32 dirty = { doc.image.w, doc.image.h, 0, 0 };

	switch (currTool) {
		case BRUSH:
		case ERASER: {
			VecI32 MousePosRel = {
				(i32)((x - viewport.x) / viewportScale),
				(i32)((y - viewport.y) / viewportScale)
			};
			dirty = Tool::Draw(
				MousePosRel.x, MousePosRel.y, doc.image.w, doc.image.h,
				isRounded, brushSize, currTool == BRUSH ? primaryColor : Pixel{ 0, 0, 0, 0 },
				doc.image.Layers[activeLayer].pixels
			);
			break;
		}
		case PAN: {
			viewport.x += x - MousePosLast.x;
			viewport.y += y - MousePosLast.y;
			break;
		}
		case COLOR_PICKER:
		case NONE: {
			break;
		}
	}

	MousePosLast = { x, y };
	return dirty;
}

mm_RectU32 Manager::onMouseUp(i32 x, i32 y, Doc& doc) {
	switch (currTool) {
		case BRUSH:
		case ERASER: {
			break;
		}
		case NONE:
		case COLOR_PICKER:
		case PAN: {
			break;
		}
	}

	return { doc.image.w, doc.image.h, 0, 0 };
}

void Manager::UpdateViewportScale(const Doc& doc) {
	viewportScale = viewportScale > 0.15f ? viewportScale : 0.15f;

	// Ensures That The ViewPort is Centered From The Center
	VecF32 CurrRectCenter = {
		(viewport.w / 2) + viewport.x, (viewport.h / 2) + viewport.y
	};
	VecF32 NewRectCenter = {
		(doc.image.w * viewportScale / 2) + viewport.x,
		(doc.image.h * viewportScale / 2) + viewport.y
	};
	viewport.x -= NewRectCenter.x - CurrRectCenter.x;
	viewport.y -= NewRectCenter.y - CurrRectCenter.y;

	// Update The Size Of The ViewPort
	viewport.w = doc.image.w * viewportScale;
	viewport.h = doc.image.h * viewportScale;
}
