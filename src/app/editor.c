#include "app/editor.h"
#include <string.h>

int EditorInit(editor_t* ed, uint32_t width, uint32_t height) {
	memset(ed, 0, sizeof(*ed));
	return 0;
}

int EditorInitFrom(editor_t* ed, const char* filePath) {
	return 0;
}

void EditorDestroy(editor_t* ed) {
}

void EditorOnMouseDown(editor_t* ed, int32_t x, int32_t y);
void EditorOnMouseMove(editor_t* ed, int32_t x, int32_t y);
void EditorOnMouseUp(editor_t* ed, int32_t x, int32_t y);

void EditorUpdateView(editor_t* ed) {
	ed->view.scale = ed->view.scale > 0.15f ? ed->view.scale : 0.15f;

	// Ensures That The viewRect is Centered From The Center
	float currX = (ed->view.w / 2) + ed->view.x;
	float currY = (ed->view.h / 2) + ed->view.y;

	float newX = (ed->canvas.image.width * ed->view.scale / 2) + ed->view.x;
	float newY = (ed->canvas.image.height * ed->view.scale / 2) + ed->view.y;

	ed->view.x -= newY - currX;
	ed->view.y -= newY - currY;

	// Update The Size Of The viewRect
	ed->view.w = ed->canvas.image.width * ed->view.scale;
	ed->view.h = ed->canvas.image.height * ed->view.scale;
}

int EditorSetFilePath(editor_t* ed, const char* filePath);
