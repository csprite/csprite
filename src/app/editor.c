#include "app/editor.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int EditorInit(editor_t* ed, uint32_t width, uint32_t height) {
	memset(ed, 0, sizeof(*ed));

	if (ImageInit(&ed->canvas.image, width, height)) {
		return 1;
	}

	if ((ed->canvas.texture = TextureInit(width, height)) == 0) {
		return 1;
	}

	ed->tool.brush.color = (pixel_t){ 255, 255, 255, 255 };
	ed->tool.brush.rounded = false;
	ed->tool.brush.size = 1;
	ed->tool.type.current = TOOL_BRUSH;
	ed->view.scale = 1.5f;
	ed->file.path = malloc(sizeof("untitled.png"));
	ed->file.name = ed->file.path;
	strncpy(ed->file.path, "untitled.png", sizeof("untitled.png"));
	EditorUpdateView(ed);

	return 0;
}

int EditorInitFrom(editor_t* ed, const char* filePath) {
	image_t img;
	if (ImageInitFrom(&img, filePath)) {
		return 1;
	}

	EditorInit(ed, img.width, img.height);
	ImageDestroy(&ed->canvas.image);
	ed->canvas.image = img;
	TextureUpdate(ed->canvas.texture, 0, 0, ed->canvas.image.width, ed->canvas.image.height, ed->canvas.image.height, (unsigned char*)ed->canvas.image.pixels);

	return 0;
}

void EditorDestroy(editor_t* ed) {
	ImageDestroy(&ed->canvas.image);
	TextureDestroy(ed->canvas.texture);
	free(ed->file.path);
}

mmRect_t plotLine(int x0, int y0, int x1, int y1, image_t* img, pixel_t color) {
	mmRect_t dirty = { img->width, img->height, 0, 0 };

	int dx  =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy  = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;

	for (;;) {
		if (x0 > -1 && y0 > -1 && x0 < img->width && y0 < img->height) {
			img->pixels[(y0 * img->width) + x0] = color;
			if (dirty.min_x > x0) dirty.min_x = x0;
			if (dirty.min_y > y0) dirty.min_y = y0;
			if (dirty.max_x < x0) dirty.max_x = x0;
			if (dirty.max_y < y0) dirty.max_y = y0;
		}
		if (x0 == x1 && y0 == y1) {
			break;
		}

		e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}

	return dirty;
}

mmRect_t EditorOnMouseDown(editor_t* ed, int32_t x, int32_t y) {
    ed->mouse.down.x = x;
    ed->mouse.down.y = y;
	ed->mouse.last.x = x;
	ed->mouse.last.y = y;

	int32_t MouseRelX = (int32_t)((x - ed->view.x) / ed->view.scale);
	int32_t MouseRelY = (int32_t)((y - ed->view.y) / ed->view.scale);

	mmRect_t dirty = { ed->canvas.image.width, ed->canvas.image.height, 0, 0 };
	if (MouseRelX < 0 || MouseRelY < 0 || MouseRelX >= ed->canvas.image.width || MouseRelY >= ed->canvas.image.height) return dirty;

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			pixel_t color = ed->tool.type.current == TOOL_BRUSH ? ed->tool.brush.color : (pixel_t){0, 0, 0, 0};
			ed->canvas.image.pixels[(MouseRelY * ed->canvas.image.width) + MouseRelX] = color;
			dirty.min_x = MouseRelX;
			dirty.min_y = MouseRelY;
			dirty.max_x = MouseRelX + 1;
			dirty.max_y = MouseRelY + 1;
			break;
		}
		case TOOL_NONE:
		case TOOL_PAN: {
			break;
		}
	}

	return dirty;
}

mmRect_t EditorOnMouseMove(editor_t* ed, int32_t x, int32_t y) {
	mmRect_t dirty = { ed->canvas.image.width, ed->canvas.image.height, 0, 0 };

	int32_t MouseRelX = (int32_t)((x - ed->view.x) / ed->view.scale);
	int32_t MouseRelY = (int32_t)((y - ed->view.y) / ed->view.scale);
	if (MouseRelX < 0 || MouseRelY < 0 || MouseRelX >= ed->canvas.image.width || MouseRelY >= ed->canvas.image.height) return dirty;

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			pixel_t color = ed->tool.type.current != TOOL_ERASER ? ed->tool.brush.color : (pixel_t){0, 0, 0, 0};
			ed->canvas.image.pixels[(MouseRelY * ed->canvas.image.width) + MouseRelX] = color;
			dirty.min_x = MouseRelX;
			dirty.min_y = MouseRelY;
			dirty.max_x = MouseRelX + 1;
			dirty.max_y = MouseRelY + 1;

			if (ed->mouse.last.x != INT_MIN && ed->mouse.last.y != INT_MIN) {
				mmRect_t newDirty = plotLine(
					(int)((ed->mouse.last.x - ed->view.x)/ed->view.scale), (int)((ed->mouse.last.y - ed->view.y)/ed->view.scale),
					MouseRelX, MouseRelY, &ed->canvas.image, color
				);

				if (newDirty.min_x < dirty.min_x) dirty.min_x = newDirty.min_x;
				if (newDirty.min_y < dirty.min_y) dirty.min_y = newDirty.min_y;
				if (newDirty.max_x > dirty.max_x) dirty.max_x = newDirty.max_x;
				if (newDirty.max_y > dirty.max_y) dirty.max_y = newDirty.max_y;
			}

			break;
		}
		case TOOL_PAN: {
			ed->view.x += x - ed->mouse.last.x;
			ed->view.y += y - ed->mouse.last.y;
			break;
		}
		case TOOL_NONE: {
			break;
		}
	}

	ed->mouse.last.x = x;
	ed->mouse.last.y = y;
	return dirty;
}

mmRect_t EditorOnMouseUp(editor_t* ed, int32_t x, int32_t y) {
	ed->mouse.down.x = INT_MIN;
	ed->mouse.down.y = INT_MIN;
	ed->mouse.last.x = INT_MIN;
	ed->mouse.last.y = INT_MIN;

	mmRect_t dirty = { ed->canvas.image.width, ed->canvas.image.height, 0, 0 };
	return dirty;
}

void EditorUpdateView(editor_t* ed) {
	ed->view.scale = ed->view.scale > 0.15f ? ed->view.scale : 0.05f;

	// Ensures That The viewRect is Centered From The Center
	float currX = (ed->view.w / 2) + ed->view.x;
	float currY = (ed->view.h / 2) + ed->view.y;

	float newX = (ed->canvas.image.width * ed->view.scale / 2) + ed->view.x;
	float newY = (ed->canvas.image.height * ed->view.scale / 2) + ed->view.y;

	ed->view.x -= newX - currX;
	ed->view.y -= newY - currY;

	// Update The Size Of The viewRect
	ed->view.w = ed->canvas.image.width * ed->view.scale;
	ed->view.h = ed->canvas.image.height * ed->view.scale;
}

int EditorSetFilePath(editor_t* ed, const char* filePath);
