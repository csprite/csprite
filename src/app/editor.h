#ifndef CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#define CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#pragma once

#include <stdbool.h>
#include "base/types.h"
#include "app/texture.h"
#include "image/image.h"

typedef enum {
	TOOL_BRUSH,
	TOOL_ERASER,
	TOOL_LINE,
	TOOL_RECT,
	TOOL_ELLIPSE,
	TOOL_PAN,
	TOOL_NONE
} Tool;

static inline const char* ToolToString(Tool t) {
	switch (t) {
		case TOOL_BRUSH:   return "Brush"; break;
		case TOOL_ERASER:  return "Eraser"; break;
		case TOOL_LINE:    return "Line"; break;
		case TOOL_RECT:    return "Rect"; break;
		case TOOL_ELLIPSE: return "Ellipse"; break;
		case TOOL_PAN:     return "Pan"; break;
		case TOOL_NONE:    return "None"; break;
	}
	return "<unknown>";
}

typedef struct {
	struct {
		Image image;
		Texture texture;
	} canvas;
	struct {
		char* path;
		char* name; // Points to start of basename in `path`
	} file;
	struct {
		struct {
			Pixel color;
		} brush;
		struct {
			Tool previous, current;
		} type;
	} tool;
	struct {
		float x, y, w, h, scale;
	} view;
	struct {
		Point down, last;
	} mouse;
} Editor;

int Editor_Init(Editor* ed, uint32_t width, uint32_t height);
int Editor_InitFrom(Editor* ed, const char* filePath);
void Editor_Deinit(Editor* ed);

void Editor_ProcessInput(Editor* ed);

Rect Editor_OnMouseDown(Editor* ed, int32_t x, int32_t y); // When Left Mouse Goes Down
Rect Editor_OnMouseMove(Editor* ed, int32_t x, int32_t y); // When Left Mouse Moves
void Editor_OnMouseDrag(Editor* ed, int32_t x, int32_t y); // From When Left Mouse Moves Until Mouse Is Released
Rect Editor_OnMouseUp(Editor* ed, int32_t x, int32_t y);   // When Mouse Is Released

void Editor_ZoomOut(Editor* ed);
void Editor_ZoomIn(Editor* ed);
void Editor_CenterView(Editor* ed, Size boundingRect);

void Editor_UpdateView(Editor* ed); // When ed.view.scale changes
int Editor_SetFilepath(Editor* ed, const char* filePath);

#endif
