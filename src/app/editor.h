#ifndef CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#define CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#pragma once

#include "base/types.h"
#include "base/arena.h"
#include "base/math.h"
#include "app/render.h"
#include "raster/raster.h"
#include "raster/math.h"

typedef enum {
	TOOL_BRUSH,
	TOOL_ERASER,
	TOOL_LINE,
	TOOL_RECT,
	TOOL_ELLIPSE,
	TOOL_PAN,
	TOOL_NONE
} EdTool;

typedef enum {
	EdMouseBtn_Left,
	EdMouseBtn_Middle,
	EdMouseBtn_Right,
	EdMouseBtn_COUNT
} EdMouseBtn;

typedef enum {
	EdMouseEvt_Press,
	EdMouseEvt_Move,
	EdMouseEvt_Release,
	EdMouseEvt_COUNT,
} EdMouseEvt;

typedef struct {
	Arena arena;

	EdTool tool_curr;
	EdTool tool_prev;
	RGBAU8 tool_color;
	B32    tool_fill;
	U32    tool_size;

	Vec2S32  view_pos;
	Rect     view_size;
	F32      view_scale;

	Vec2S32  mouse_down;
	Vec2S32  mouse_last;

	Raster   image;
	R_Handle image_tex;
	R_Handle checker_tex;
} Editor;

Editor Editor_Init(U32 width, U32 height);
// Editor Editor_InitFrom(Arena* a, const char* filePath);
void Editor_Deinit(Editor* ed);

void Editor_ProcessInput(Editor* ed);

void Editor_ZoomOut(Editor* ed);
void Editor_ZoomIn(Editor* ed);
void Editor_CenterView(Editor* ed, Rect boundingRect);
void Editor_UpdateView(Editor* ed); // When ed.view.scale changes

const char* Editor_ToolGetHumanReadable(EdTool t);
S32 Editor_SetFilepath(Editor* ed, const char* filePath);

#endif
