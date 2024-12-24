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
} tool_t;

static inline const char* ToolToString(tool_t t) {
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
		image_t image;
		texture_t texture;
	} canvas;
	struct {
		char* path;
		char* name; // Points to start of basename in `path`
	} file;
	struct {
		struct {
			pixel_t color;
		} brush;
		struct {
			tool_t previous, current;
		} type;
	} tool;
	struct {
		float x, y, w, h, scale;
	} view;
	struct {
		Vec2_t down, last;
	} mouse;
} editor_t;

int editor_init(editor_t* ed, uint32_t width, uint32_t height);
int editor_initFrom(editor_t* ed, const char* filePath);
void editor_deinit(editor_t* ed);

void editor_process_input(editor_t* ed);

Rect_t editor_on_mouse_down(editor_t* ed, int32_t x, int32_t y); // When Left Mouse Goes Down
Rect_t editor_on_mouse_move(editor_t* ed, int32_t x, int32_t y); // When Left Mouse Moves
void   editor_on_mouse_drag(editor_t* ed, int32_t x, int32_t y); // From When Left Mouse Moves Until Mouse Is Released
Rect_t editor_on_mouse_up(editor_t* ed, int32_t x, int32_t y);   // When Mouse Is Released

void editor_zoom_out(editor_t* ed);
void editor_zoom_in(editor_t* ed);
void editor_center_view(editor_t* ed, Vec2_t boundingRect);

void editor_update_view(editor_t* ed); // When ed.view.scale changes
int editor_set_filepath(editor_t* ed, const char* filePath);

#endif
