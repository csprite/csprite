#ifndef CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#define CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#pragma once

#include "base/types.h"
#include "base/arena.h"
#include "base/math.h"
#include "app/render.h"
#include "bitmap/bitmap.h"

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
		Bitmap  image;
		R_Handle texture;
		R_Handle checker;
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
	Arena arena;
} Editor;

Editor Editor_Init(U32 width, U32 height);
// Editor Editor_InitFrom(Arena* a, const char* filePath);
void Editor_Deinit(Editor* ed);

void Editor_ProcessInput(Editor* ed);

Rng2D Editor_OnMouseDown(Editor* ed, S32 x, S32 y); // When Left Mouse Goes Down
Rng2D Editor_OnMouseMove(Editor* ed, S32 x, S32 y); // When Left Mouse Moves
void Editor_DrawToolPreview(Editor* ed, S32 x, S32 y);
Rng2D Editor_OnMouseUp(Editor* ed, S32 x, S32 y);   // When Mouse Is Released

void Editor_ZoomOut(Editor* ed);
void Editor_ZoomIn(Editor* ed);
void Editor_CenterView(Editor* ed, Rect boundingRect);

void Editor_UpdateView(Editor* ed); // When ed.view.scale changes
S32 Editor_SetFilepath(Editor* ed, const char* filePath);

#endif
