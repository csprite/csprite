#include "app/editor.h"
#include "gfx/gfx.h"
#include "cimgui.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

int EditorInit(editor_t* ed, uint32_t width, uint32_t height) {
	*ed = (editor_t){0};

	if (ImageInit(&ed->canvas.image, width, height)) {
		return 1;
	}

	if ((ed->canvas.texture = TextureInit(width, height)) == 0) {
		return 1;
	}

	ed->tool.brush.color = (pixel_t){ 255, 255, 255, 255 };
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

Rect_t EditorOnMouseDown(editor_t* ed, int32_t x, int32_t y) {
    ed->mouse.down.x = x;
    ed->mouse.down.y = y;
	ed->mouse.last.x = x;
	ed->mouse.last.y = y;

	int32_t MouseRelX = (int32_t)((x - ed->view.x) / ed->view.scale);
	int32_t MouseRelY = (int32_t)((y - ed->view.y) / ed->view.scale);

	Rect_t dirty = {0};
	if (MouseRelX < 0 || MouseRelY < 0 || MouseRelX >= ed->canvas.image.width || MouseRelY >= ed->canvas.image.height) return dirty;

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			pixel_t color = ed->tool.type.current == TOOL_BRUSH ? ed->tool.brush.color : (pixel_t){0, 0, 0, 0};
			ed->canvas.image.pixels[(MouseRelY * ed->canvas.image.width) + MouseRelX] = color;
			dirty.start.x = MouseRelX;
			dirty.start.y = MouseRelY;
			dirty.end.x = MouseRelX + 1;
			dirty.end.y = MouseRelY + 1;
			break;
		}
		case TOOL_LINE:
		case TOOL_RECT:
		case TOOL_ELLIPSE:
		case TOOL_NONE:
		case TOOL_PAN: {
			break;
		}
	}

	return dirty;
}

void EditorOnMouseDrag(editor_t* ed, int32_t x, int32_t y) {
	int32_t MouseRelX = (int32_t)((x - ed->view.x) / ed->view.scale);
	int32_t MouseRelY = (int32_t)((y - ed->view.y) / ed->view.scale);

	int MouseDownRelX = (ed->mouse.down.x - ed->view.x) / ed->view.scale;
	int MouseDownRelY = (ed->mouse.down.y - ed->view.y) / ed->view.scale;

	Vec2_t TopLeft = { MouseDownRelX, MouseDownRelY };
	Vec2_t BotRight = { MouseRelX, MouseRelY };

	ensureRectCoords(&TopLeft, &BotRight);

	Vec2_t BotLeft = { TopLeft.x, BotRight.y };
	Vec2_t TopRight = { BotRight.x, TopLeft.y };

	switch (ed->tool.type.current) {
		case TOOL_LINE: {
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (MouseDownRelX * ed->view.scale) + ed->view.x, (MouseDownRelY * ed->view.scale) + ed->view.y },
				(ImVec2){ ((MouseDownRelX + 1) * ed->view.scale) + ed->view.x, ((MouseDownRelY + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddLine(
				igGetForegroundDrawList_Nil(),
				(ImVec2){ ((MouseDownRelX + 0.5) * ed->view.scale) + ed->view.x, ((MouseDownRelY + 0.5) * ed->view.scale) + ed->view.y },
				(ImVec2){ ((MouseRelX + 0.5) * ed->view.scale) + ed->view.x, ((MouseRelY + 0.5) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color,
				ed->view.scale / 3
			);
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (MouseRelX * ed->view.scale) + ed->view.x, (MouseRelY * ed->view.scale) + ed->view.y },
				(ImVec2){ ((MouseRelX + 1) * ed->view.scale) + ed->view.x, ((MouseRelY + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);
			break;
		}
		case TOOL_RECT:
		case TOOL_ELLIPSE: {
			ImDrawList_AddRectFilled( // Top Left
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopLeft.x * ed->view.scale) + ed->view.x, (TopLeft.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((TopLeft.x + 1) * ed->view.scale) + ed->view.x, ((TopLeft.y + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddRectFilled( // Top Right
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopRight.x * ed->view.scale) + ed->view.x, (TopRight.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((TopRight.x + 1) * ed->view.scale) + ed->view.x, ((TopRight.y + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddRectFilled( // Bottom Left
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (BotLeft.x * ed->view.scale) + ed->view.x, (BotLeft.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotLeft.x + 1) * ed->view.scale) + ed->view.x, ((BotLeft.y + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddRectFilled( // Bottom Right
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (BotRight.x * ed->view.scale) + ed->view.x, (BotRight.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotRight.x + 1) * ed->view.scale) + ed->view.x, ((BotRight.y + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);

			if (ed->tool.type.current == TOOL_RECT) {
				ImDrawList_AddRect(
				    igGetForegroundDrawList_Nil(),
					(ImVec2){ ((TopLeft.x + 0.5) * ed->view.scale) + ed->view.x, ((TopLeft.y + 0.5) * ed->view.scale) + ed->view.y },
					(ImVec2){ ((BotRight.x + 0.5) * ed->view.scale) + ed->view.x, ((BotRight.y + 0.5) * ed->view.scale) + ed->view.y },
					*(uint32_t*)&ed->tool.brush.color, 0, 0, ed->view.scale / 4
				);
			} else {
				ImDrawList_AddEllipse(
					igGetForegroundDrawList_Nil(),
					(ImVec2){
						( ( TopLeft.x + 0.5 + ( (float)(TopRight.x - TopLeft.x) / 2) ) * ed->view.scale ) + ed->view.x,
						( ( TopLeft.y + 0.5 + ( (float)(BotLeft.y - TopLeft.y) / 2) ) * ed->view.scale ) + ed->view.y,
					},
					(ImVec2){ ((TopRight.x - TopLeft.x) * ed->view.scale)/2, ((TopLeft.y - BotLeft.y) * ed->view.scale)/2 },
					*(uint32_t*)&ed->tool.brush.color, 0, 0, 1
				);
			}
			break;
		}
		case TOOL_BRUSH:
		case TOOL_ERASER:
		case TOOL_PAN:
		case TOOL_NONE: {
			break;
		}
	}
}

Rect_t EditorOnMouseMove(editor_t* ed, int32_t x, int32_t y) {
	Rect_t dirty = {0};

	int32_t MouseRelX = (int32_t)((x - ed->view.x) / ed->view.scale);
	int32_t MouseRelY = (int32_t)((y - ed->view.y) / ed->view.scale);

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			if (MouseRelX < 0 || MouseRelY < 0 || MouseRelX > ed->canvas.image.width || MouseRelY > ed->canvas.image.height) break;

			pixel_t color = ed->tool.type.current != TOOL_ERASER ? ed->tool.brush.color : (pixel_t){0, 0, 0, 0};
			Rect_t newDirty = plotLine(
				(Vec2_t){ (int64_t)((ed->mouse.last.x - ed->view.x)/ed->view.scale), (int64_t)((ed->mouse.last.y - ed->view.y)/ed->view.scale) },
				(Vec2_t){ MouseRelX, MouseRelY },
				&ed->canvas.image, color
			);

			if (newDirty.start.x < dirty.start.x) dirty.start.x = newDirty.start.x;
			if (newDirty.start.y < dirty.start.y) dirty.start.y = newDirty.start.y;
			if (newDirty.end.x > dirty.end.x) dirty.end.x = newDirty.end.x;
			if (newDirty.end.y > dirty.end.y) dirty.end.y = newDirty.end.y;
			break;
		}
		case TOOL_PAN: {
			ed->view.x += x - ed->mouse.last.x;
			ed->view.y += y - ed->mouse.last.y;
			break;
		}
		case TOOL_LINE:
		case TOOL_RECT:
		case TOOL_ELLIPSE:
		case TOOL_NONE: {
			break;
		}
	}

	ed->mouse.last.x = x;
	ed->mouse.last.y = y;
	return dirty;
}

Rect_t EditorOnMouseUp(editor_t* ed, int32_t x, int32_t y) {
	Rect_t dirty = {0};

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER:
		case TOOL_PAN:
		case TOOL_NONE: {
			break;
		}
		case TOOL_LINE:
		case TOOL_RECT:
		case TOOL_ELLIPSE: {
			int32_t MouseRelX = ((x - ed->view.x) / ed->view.scale);
			int32_t MouseRelY = ((y - ed->view.y) / ed->view.scale);
			int32_t MouseDownRelX = (ed->mouse.down.x - ed->view.x) / ed->view.scale;
			int32_t MouseDownRelY = (ed->mouse.down.y - ed->view.y) / ed->view.scale;

			dirty = ed->tool.type.current == TOOL_LINE ?
					plotLine((Vec2_t){ MouseDownRelX, MouseDownRelY }, (Vec2_t){ MouseRelX, MouseRelY }, &ed->canvas.image, ed->tool.brush.color) :
						ed->tool.type.current == TOOL_RECT ?
							plotRect((Vec2_t){ MouseDownRelX, MouseDownRelY }, (Vec2_t){ MouseRelX, MouseRelY }, &ed->canvas.image, ed->tool.brush.color) :
							plotEllipseRect((Vec2_t){ MouseDownRelX, MouseDownRelY }, (Vec2_t){ MouseRelX, MouseRelY }, &ed->canvas.image, ed->tool.brush.color);
			break;
		}
	}

	ed->mouse.down.x = ed->mouse.down.y = ed->mouse.last.x = ed->mouse.last.y = 0;

	return dirty;
}

void EditorUpdateView(editor_t* ed) {
	ed->view.scale = ed->view.scale < 0.01 ? 0.01 : ed->view.scale;

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

void EditorZoomOut(editor_t* ed) {
	if (ed->view.scale > 1) {
		ed->view.scale -= 0.15;
	} else {
		ed->view.scale -= 0.05;
	}

	ed->view.scale = ed->view.scale <= 0.05 ? 0.05 : ed->view.scale;
	EditorUpdateView(ed);
}

void EditorZoomIn(editor_t* ed) {
	if (ed->view.scale > 1) {
		ed->view.scale += 0.15;
	} else {
		ed->view.scale += 0.05;
	}

	EditorUpdateView(ed);
}

void EditorProcessInput(editor_t* ed) {
	ImGuiIO* io = igGetIO();

	if (!igIsMouseDown_Nil(ImGuiMouseButton_Left)) {
		if (io->MouseWheel > 0) EditorZoomIn(ed);
		else if (io->MouseWheel < 0) EditorZoomOut(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiMod_Ctrl | ImGuiKey_Equal)) EditorZoomIn(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiMod_Ctrl | ImGuiKey_Minus)) EditorZoomOut(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiKey_B)) ed->tool.type.current = TOOL_BRUSH;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_E)) ed->tool.type.current = TOOL_ERASER;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_L)) ed->tool.type.current = TOOL_LINE;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_R)) ed->tool.type.current = TOOL_RECT;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_C)) ed->tool.type.current = TOOL_ELLIPSE;
		else if (igIsKeyPressed_Bool(ImGuiKey_Space, false)) { ed->tool.type.previous = ed->tool.type.current;ed->tool.type.current = TOOL_PAN; }
		else if (igIsKeyReleased_Nil(ImGuiKey_Space)) { ed->tool.type.current = ed->tool.type.previous; }
	}

	Rect_t dirty = {0};

	if (igIsMouseClicked_Bool(ImGuiMouseButton_Left, false)) {
		dirty = EditorOnMouseDown(ed, io->MousePos.x, io->MousePos.y);
	} else {
		int32_t MouseRelX = (int32_t)((io->MousePos.x - ed->view.x) / ed->view.scale);
		int32_t MouseRelY = (int32_t)((io->MousePos.y - ed->view.y) / ed->view.scale);
		ImDrawList_AddRect(
		    igGetForegroundDrawList_Nil(),
			(ImVec2){ (MouseRelX * ed->view.scale) + ed->view.x, (MouseRelY * ed->view.scale) + ed->view.y },
			(ImVec2){ ((MouseRelX + 1) * ed->view.scale) + ed->view.x, ((MouseRelY + 1) * ed->view.scale) + ed->view.y },
			*(uint32_t*)&ed->tool.brush.color, 0, 0, 1
		);
	}

	if (igIsMouseDown_Nil(ImGuiMouseButton_Left) && (io->MouseDelta.x != 0 || io->MouseDelta.y != 0)) {
		dirty = EditorOnMouseMove(ed, io->MousePos.x, io->MousePos.y);
	}
	if (igIsMouseDragging(ImGuiMouseButton_Left, -1)) {
		EditorOnMouseDrag(ed, io->MousePos.x, io->MousePos.y);
	}
	if (igIsMouseReleased_Nil(ImGuiMouseButton_Left)) {
		dirty = EditorOnMouseUp(ed, io->MousePos.x, io->MousePos.y);
	}

	if (rect_is_valid(&dirty)) {
		TextureUpdate(
			ed->canvas.texture, dirty.start.x, dirty.start.y,
			dirty.end.x - dirty.start.x, dirty.end.y - dirty.start.y,
			ed->canvas.image.width, (unsigned char*)ed->canvas.image.pixels
		);
		rect_invalidate(&dirty);
	}
}
