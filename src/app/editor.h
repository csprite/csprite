#ifndef CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#define CSPRITE_SRC_APP_EDITOR_H_INCLUDED_
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "pixel.h"
#include "app/texture.h"

typedef struct {
	struct {
		pixel_t* pixels;
		texture_t texture;
		uint32_t width, height;
	} image;
	struct {
		char* path;
		char* name;
	} file;
	struct {
		bool rounded;
		uint32_t size;
		pixel_t color;
	} brush;
	struct {
		float x, y, w, h, scale;
	} view;
	struct {
		struct {
			int32_t x, y;
		} down, last;
	} mouse;
} editor_t;

int EditorInit(editor_t* ed);
void EditorDestroy(editor_t* ed);

void EditorOnMouseDown(editor_t* ed, int32_t x, int32_t y);
void EditorOnMouseMove(editor_t* ed, int32_t x, int32_t y);
void EditorOnMouseUp(editor_t* ed, int32_t x, int32_t y);
void EditorUpdateView(editor_t* ed); // When ed.view.scale changes
int EditorSetFilePath(editor_t* ed, const char* filePath);

#endif
