#include "app/editor.h"
#include "base/memory.h"
#include "gfx/gfx.h"
#include "imgui.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

S32 Editor_Init(Editor* ed, U32 width, U32 height) {
	*ed = (Editor){0};

	Image_Init(&ed->canvas.image, width, height);
	ed->canvas.texture = Texture_Init(width, height);
	ed->tool.brush.color = (Pixel){ 255, 255, 255, 255 };
	ed->tool.type.current = TOOL_BRUSH;
	ed->view.scale = 1.5f;
	ed->file.path = NULL;
	ed->file.name = NULL;
	Editor_UpdateView(ed);

	return 0;
}

S32 Editor_InitFrom(Editor* ed, const char* filePath) {
	Image img;
	if (Image_InitFrom(&img, filePath)) {
		return 1;
	}

	Editor_Init(ed, img.width, img.height);
	Image_Deinit(&ed->canvas.image);
	ed->canvas.image = img;
	Texture_Update(ed->canvas.texture, 0, 0, ed->canvas.image.width, ed->canvas.image.height, ed->canvas.image.height, (unsigned char*)ed->canvas.image.pixels);

	S32 len = strlen(filePath) + 1;
	ed->file.path = Memory_Alloc(len);
	strncpy(ed->file.path, filePath, len);

	return 0;
}

void Editor_Deinit(Editor* ed) {
	Image_Deinit(&ed->canvas.image);
	Texture_Deinit(ed->canvas.texture);

	if (ed->file.path) {
		Memory_Dealloc(ed->file.path);
	}
}

Rect Editor_OnMouseDown(Editor* ed, S32 x, S32 y) {
    ed->mouse.down.x = x;
    ed->mouse.down.y = y;
	ed->mouse.last.x = x;
	ed->mouse.last.y = y;

	S32 MouseRelX = (S32)((x - ed->view.x) / ed->view.scale);
	S32 MouseRelY = (S32)((y - ed->view.y) / ed->view.scale);

	Rect dirty = {0};
	if (MouseRelX < 0 || MouseRelY < 0 || MouseRelX >= ed->canvas.image.width || MouseRelY >= ed->canvas.image.height) return dirty;

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			Pixel color = ed->tool.type.current == TOOL_BRUSH ? ed->tool.brush.color : (Pixel){0, 0, 0, 0};
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

void Editor_OnMouseDrag(Editor* ed, S32 x, S32 y) {
	S32 MouseRelX = (S32)((x - ed->view.x) / ed->view.scale);
	S32 MouseRelY = (S32)((y - ed->view.y) / ed->view.scale);

	S32 MouseDownRelX = (ed->mouse.down.x - ed->view.x) / ed->view.scale;
	S32 MouseDownRelY = (ed->mouse.down.y - ed->view.y) / ed->view.scale;

	Point TopLeft = { MouseDownRelX, MouseDownRelY };
	Point BotRight = { MouseRelX, MouseRelY };

	ensureRectCoords(&TopLeft, &BotRight);

	Point BotLeft = { TopLeft.x, BotRight.y };
	Point TopRight = { BotRight.x, TopLeft.y };

	switch (ed->tool.type.current) {
		case TOOL_LINE: {
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (MouseDownRelX * ed->view.scale) + ed->view.x, (MouseDownRelY * ed->view.scale) + ed->view.y },
				(ImVec2){ ((MouseDownRelX + 1) * ed->view.scale) + ed->view.x, ((MouseDownRelY + 1) * ed->view.scale) + ed->view.y },
				*(U32*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddLine(
				igGetForegroundDrawList_Nil(),
				(ImVec2){ ((MouseDownRelX + 0.5) * ed->view.scale) + ed->view.x, ((MouseDownRelY + 0.5) * ed->view.scale) + ed->view.y },
				(ImVec2){ ((MouseRelX + 0.5) * ed->view.scale) + ed->view.x, ((MouseRelY + 0.5) * ed->view.scale) + ed->view.y },
				*(U32*)&ed->tool.brush.color,
				ed->view.scale / 3
			);
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (MouseRelX * ed->view.scale) + ed->view.x, (MouseRelY * ed->view.scale) + ed->view.y },
				(ImVec2){ ((MouseRelX + 1) * ed->view.scale) + ed->view.x, ((MouseRelY + 1) * ed->view.scale) + ed->view.y },
				*(U32*)&ed->tool.brush.color, 0, 0
			);
			break;
		}
		case TOOL_RECT: {
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopLeft.x * ed->view.scale) + ed->view.x, (TopLeft.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotRight.x + 1) * ed->view.scale) + ed->view.x, ((BotRight.y + 1) * ed->view.scale) + ed->view.y },
				*(U32*)&ed->tool.brush.color, 0, 0
			);
			break;
		}
		case TOOL_ELLIPSE: {
			ImDrawList_AddRectFilled( // Top Left
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopLeft.x * ed->view.scale) + ed->view.x, (TopLeft.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((TopLeft.x + 1) * ed->view.scale) + ed->view.x, ((TopLeft.y + 1) * ed->view.scale) + ed->view.y },
				*(U32*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddRectFilled( // Top Right
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopRight.x * ed->view.scale) + ed->view.x, (TopRight.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((TopRight.x + 1) * ed->view.scale) + ed->view.x, ((TopRight.y + 1) * ed->view.scale) + ed->view.y },
				*(U32*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddRectFilled( // Bottom Left
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (BotLeft.x * ed->view.scale) + ed->view.x, (BotLeft.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotLeft.x + 1) * ed->view.scale) + ed->view.x, ((BotLeft.y + 1) * ed->view.scale) + ed->view.y },
				*(U32*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddRectFilled( // Bottom Right
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (BotRight.x * ed->view.scale) + ed->view.x, (BotRight.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotRight.x + 1) * ed->view.scale) + ed->view.x, ((BotRight.y + 1) * ed->view.scale) + ed->view.y },
				*(U32*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddEllipse(
				igGetForegroundDrawList_Nil(),
				(ImVec2){
					( ( TopLeft.x + 0.5 + ( (F32)(TopRight.x - TopLeft.x) / 2) ) * ed->view.scale ) + ed->view.x,
					( ( TopLeft.y + 0.5 + ( (F32)(BotLeft.y - TopLeft.y) / 2) ) * ed->view.scale ) + ed->view.y,
				},
				(ImVec2){ ((TopRight.x - TopLeft.x) * ed->view.scale)/2, ((TopLeft.y - BotLeft.y) * ed->view.scale)/2 },
				*(U32*)&ed->tool.brush.color, 0, 0, 1
			);
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

Rect Editor_OnMouseMove(Editor* ed, S32 x, S32 y) {
	Rect dirty = {0};

	S32 MouseRelX = (S32)((x - ed->view.x) / ed->view.scale);
	S32 MouseRelY = (S32)((y - ed->view.y) / ed->view.scale);

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			if (MouseRelX < 0 || MouseRelY < 0 || MouseRelX > ed->canvas.image.width || MouseRelY > ed->canvas.image.height) break;

			Pixel color = ed->tool.type.current != TOOL_ERASER ? ed->tool.brush.color : (Pixel){0, 0, 0, 0};
			Rect newDirty = plotLine(
				(Point){ (S32)((ed->mouse.last.x - ed->view.x)/ed->view.scale), (S32)((ed->mouse.last.y - ed->view.y)/ed->view.scale) },
				(Point){ MouseRelX, MouseRelY },
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

Rect Editor_OnMouseUp(Editor* ed, S32 x, S32 y) {
	Rect dirty = {0};

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
			S32 MouseRelX = ((x - ed->view.x) / ed->view.scale);
			S32 MouseRelY = ((y - ed->view.y) / ed->view.scale);
			S32 MouseDownRelX = (ed->mouse.down.x - ed->view.x) / ed->view.scale;
			S32 MouseDownRelY = (ed->mouse.down.y - ed->view.y) / ed->view.scale;

			if (ed->tool.type.current == TOOL_LINE) {
				dirty = plotLine((Point){ MouseDownRelX, MouseDownRelY }, (Point){ MouseRelX, MouseRelY }, &ed->canvas.image, ed->tool.brush.color);
			} else if (ed->tool.type.current == TOOL_RECT) {
				dirty = plotRect((Point){ MouseDownRelX, MouseDownRelY }, (Point){ MouseRelX, MouseRelY }, &ed->canvas.image, ed->tool.brush.color);
			} else {
				dirty = plotEllipseRect((Point){ MouseDownRelX, MouseDownRelY }, (Point){ MouseRelX, MouseRelY }, &ed->canvas.image, ed->tool.brush.color);
			}
			break;
		}
	}

	ed->mouse.down.x = ed->mouse.down.y = ed->mouse.last.x = ed->mouse.last.y = 0;

	return dirty;
}

void Editor_UpdateView(Editor* ed) {
	ed->view.scale = ed->view.scale < 0.01 ? 0.01 : ed->view.scale;

	// Ensures That The viewRect is Centered From The Center
	F32 currX = (ed->view.w / 2) + ed->view.x;
	F32 currY = (ed->view.h / 2) + ed->view.y;

	F32 newX = (ed->canvas.image.width * ed->view.scale / 2) + ed->view.x;
	F32 newY = (ed->canvas.image.height * ed->view.scale / 2) + ed->view.y;

	ed->view.x -= newX - currX;
	ed->view.y -= newY - currY;

	// Update The Size Of The viewRect
	ed->view.w = ed->canvas.image.width * ed->view.scale;
	ed->view.h = ed->canvas.image.height * ed->view.scale;
}

S32 Editor_SetFilepath(Editor* ed, const char* filePath);

void Editor_ZoomOut(Editor* ed) {
	if (ed->view.scale > 1) {
		ed->view.scale -= 0.15;
	} else {
		ed->view.scale -= 0.05;
	}

	ed->view.scale = ed->view.scale <= 0.05 ? 0.05 : ed->view.scale;
	Editor_UpdateView(ed);
}

void Editor_ZoomIn(Editor* ed) {
	if (ed->view.scale > 1) {
		ed->view.scale += 0.15;
	} else {
		ed->view.scale += 0.05;
	}

	Editor_UpdateView(ed);
}

void Editor_CenterView(Editor* ed, Size boundingRect) {
	ed->view.x = ((F32)boundingRect.w / 2) - (ed->view.w / 2);
	ed->view.y = ((F32)boundingRect.h / 2) - (ed->view.h / 2);
}

void Editor_ProcessInput(Editor* ed) {
	ImGuiIO* io = igGetIO();

	if (!igIsMouseDown_Nil(ImGuiMouseButton_Left)) {
		if (io->MouseWheel > 0) Editor_ZoomIn(ed);
		else if (io->MouseWheel < 0) Editor_ZoomOut(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiMod_Ctrl | ImGuiKey_Equal)) Editor_ZoomIn(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiMod_Ctrl | ImGuiKey_Minus)) Editor_ZoomOut(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiKey_B)) ed->tool.type.current = TOOL_BRUSH;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_E)) ed->tool.type.current = TOOL_ERASER;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_L)) ed->tool.type.current = TOOL_LINE;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_R)) ed->tool.type.current = TOOL_RECT;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_C)) ed->tool.type.current = TOOL_ELLIPSE;
		else if (igIsKeyPressed_Bool(ImGuiKey_Space, false)) { ed->tool.type.previous = ed->tool.type.current;ed->tool.type.current = TOOL_PAN; }
		else if (igIsKeyReleased_Nil(ImGuiKey_Space)) { ed->tool.type.current = ed->tool.type.previous; }
	}

	Rect dirty = {0};

	if (igIsMouseClicked_Bool(ImGuiMouseButton_Left, false)) {
		dirty = Editor_OnMouseDown(ed, io->MousePos.x, io->MousePos.y);
	} else {
		S32 MouseRelX = (S32)((io->MousePos.x - ed->view.x) / ed->view.scale);
		S32 MouseRelY = (S32)((io->MousePos.y - ed->view.y) / ed->view.scale);
		ImDrawList_AddRect(
		    igGetForegroundDrawList_Nil(),
			(ImVec2){ (MouseRelX * ed->view.scale) + ed->view.x, (MouseRelY * ed->view.scale) + ed->view.y },
			(ImVec2){ ((MouseRelX + 1) * ed->view.scale) + ed->view.x, ((MouseRelY + 1) * ed->view.scale) + ed->view.y },
			*(U32*)&ed->tool.brush.color, 0, 0, 1
		);
	}

	if (igIsMouseDown_Nil(ImGuiMouseButton_Left) && (io->MouseDelta.x != 0 || io->MouseDelta.y != 0)) {
		dirty = Editor_OnMouseMove(ed, io->MousePos.x, io->MousePos.y);
	}
	if (igIsMouseDragging(ImGuiMouseButton_Left, -1)) {
		Editor_OnMouseDrag(ed, io->MousePos.x, io->MousePos.y);
	}
	if (igIsMouseReleased_Nil(ImGuiMouseButton_Left)) {
		dirty = Editor_OnMouseUp(ed, io->MousePos.x, io->MousePos.y);
	}

	if (Rect_IsValid(dirty)) {
		Texture_Update(
			ed->canvas.texture, dirty.start.x, dirty.start.y,
			dirty.end.x - dirty.start.x, dirty.end.y - dirty.start.y,
			ed->canvas.image.width, (U8*)ed->canvas.image.pixels
		);
		Rect_Invalidate(dirty);
	}
}
