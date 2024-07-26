#include "app/editor.h"
#include <stdlib.h>
#include <string.h>

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
	ed->tool.type.current = TOOL_NONE;
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
	TextureUpdate(ed->canvas.texture, ed->canvas.image.width, ed->canvas.image.height, (unsigned char*)ed->canvas.image.pixels);

	return 0;
}

void EditorDestroy(editor_t* ed) {
	ImageDestroy(&ed->canvas.image);
	free(ed->file.path);
}

mmRect_t EditorOnMouseDown(editor_t* ed, int32_t x, int32_t y) {
    ed->mouse.down.x = x;
    ed->mouse.down.y = y;
	ed->mouse.last.x = x;
	ed->mouse.last.y = y;

	int32_t MouseRelX = (int32_t)((x - ed->view.x) / ed->view.scale);
	int32_t MouseRelY = (int32_t)((y - ed->view.y) / ed->view.scale);

	mmRect_t dirty = { ed->canvas.image.width, ed->canvas.image.height, 0, 0 };

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
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

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
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
	mmRect_t dirty = { ed->canvas.image.width, ed->canvas.image.height, 0, 0 };
	return dirty;
}

void EditorUpdateView(editor_t* ed) {
	ed->view.scale = ed->view.scale > 0.15f ? ed->view.scale : 0.15f;

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
