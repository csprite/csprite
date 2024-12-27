#ifndef CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#define CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#pragma once

#include <stdbool.h>
#include "pixel.h"
#include "types.h"
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
		Vec2 down, last;
	} mouse;
} Editor;

int editor_init(Editor* ed, uint32_t width, uint32_t height);
int editor_initFrom(Editor* ed, const char* filePath);
void editor_deinit(Editor* ed);

void editor_process_input(Editor* ed);

Rect editor_on_mouse_down(Editor* ed, int32_t x, int32_t y); // When Left Mouse Goes Down
Rect editor_on_mouse_move(Editor* ed, int32_t x, int32_t y); // When Left Mouse Moves
void   editor_on_mouse_drag(Editor* ed, int32_t x, int32_t y); // From When Left Mouse Moves Until Mouse Is Released
Rect editor_on_mouse_up(Editor* ed, int32_t x, int32_t y);   // When Mouse Is Released

void editor_zoom_out(Editor* ed);
void editor_zoom_in(Editor* ed);
void editor_center_view(Editor* ed, Vec2 boundingRect);

void editor_update_view(Editor* ed); // When ed.view.scale changes
int editor_set_filepath(Editor* ed, const char* filePath);

#endif
