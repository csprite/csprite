#include "app/editor.h"
#include "base/memory.h"
#include "gfx/gfx.h"
#include "cimgui.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

int editor_init(Editor* ed, uint32_t width, uint32_t height) {
	*ed = (Editor){0};

	image_init(&ed->canvas.image, width, height);
	ed->canvas.texture = texture_init(width, height);
	ed->tool.brush.color = (Pixel){ 255, 255, 255, 255 };
	ed->tool.type.current = TOOL_BRUSH;
	ed->view.scale = 1.5f;
	ed->file.path = NULL;
	ed->file.name = NULL;
	editor_update_view(ed);

	return 0;
}

int editor_initFrom(Editor* ed, const char* filePath) {
	Image img;
	if (image_initFrom(&img, filePath)) {
		return 1;
	}

	editor_init(ed, img.width, img.height);
	image_deinit(&ed->canvas.image);
	ed->canvas.image = img;
	texture_update(ed->canvas.texture, 0, 0, ed->canvas.image.width, ed->canvas.image.height, ed->canvas.image.height, (unsigned char*)ed->canvas.image.pixels);

	int len = strlen(filePath) + 1;
	ed->file.path = Memory_Alloc(len);
	strncpy(ed->file.path, filePath, len);

	return 0;
}

void editor_deinit(Editor* ed) {
	image_deinit(&ed->canvas.image);
	texture_deinit(ed->canvas.texture);

	if (ed->file.path) {
		Memory_Dealloc(ed->file.path);
	}
}

Rect editor_on_mouse_down(Editor* ed, int32_t x, int32_t y) {
    ed->mouse.down.x = x;
    ed->mouse.down.y = y;
	ed->mouse.last.x = x;
	ed->mouse.last.y = y;

	int32_t MouseRelX = (int32_t)((x - ed->view.x) / ed->view.scale);
	int32_t MouseRelY = (int32_t)((y - ed->view.y) / ed->view.scale);

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

void editor_on_mouse_drag(Editor* ed, int32_t x, int32_t y) {
	int32_t MouseRelX = (int32_t)((x - ed->view.x) / ed->view.scale);
	int32_t MouseRelY = (int32_t)((y - ed->view.y) / ed->view.scale);

	int MouseDownRelX = (ed->mouse.down.x - ed->view.x) / ed->view.scale;
	int MouseDownRelY = (ed->mouse.down.y - ed->view.y) / ed->view.scale;

	Vec2 TopLeft = { MouseDownRelX, MouseDownRelY };
	Vec2 BotRight = { MouseRelX, MouseRelY };

	ensureRectCoords(&TopLeft, &BotRight);

	Vec2 BotLeft = { TopLeft.x, BotRight.y };
	Vec2 TopRight = { BotRight.x, TopLeft.y };

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
		case TOOL_RECT: {
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopLeft.x * ed->view.scale) + ed->view.x, (TopLeft.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotRight.x + 1) * ed->view.scale) + ed->view.x, ((BotRight.y + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);
			break;
		}
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
			ImDrawList_AddEllipse(
				igGetForegroundDrawList_Nil(),
				(ImVec2){
					( ( TopLeft.x + 0.5 + ( (float)(TopRight.x - TopLeft.x) / 2) ) * ed->view.scale ) + ed->view.x,
					( ( TopLeft.y + 0.5 + ( (float)(BotLeft.y - TopLeft.y) / 2) ) * ed->view.scale ) + ed->view.y,
				},
				(ImVec2){ ((TopRight.x - TopLeft.x) * ed->view.scale)/2, ((TopLeft.y - BotLeft.y) * ed->view.scale)/2 },
				*(uint32_t*)&ed->tool.brush.color, 0, 0, 1
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

Rect editor_on_mouse_move(Editor* ed, int32_t x, int32_t y) {
	Rect dirty = {0};

	int32_t MouseRelX = (int32_t)((x - ed->view.x) / ed->view.scale);
	int32_t MouseRelY = (int32_t)((y - ed->view.y) / ed->view.scale);

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			if (MouseRelX < 0 || MouseRelY < 0 || MouseRelX > ed->canvas.image.width || MouseRelY > ed->canvas.image.height) break;

			Pixel color = ed->tool.type.current != TOOL_ERASER ? ed->tool.brush.color : (Pixel){0, 0, 0, 0};
			Rect newDirty = plotLine(
				(Vec2){ (int64_t)((ed->mouse.last.x - ed->view.x)/ed->view.scale), (int64_t)((ed->mouse.last.y - ed->view.y)/ed->view.scale) },
				(Vec2){ MouseRelX, MouseRelY },
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

Rect editor_on_mouse_up(Editor* ed, int32_t x, int32_t y) {
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
			int32_t MouseRelX = ((x - ed->view.x) / ed->view.scale);
			int32_t MouseRelY = ((y - ed->view.y) / ed->view.scale);
			int32_t MouseDownRelX = (ed->mouse.down.x - ed->view.x) / ed->view.scale;
			int32_t MouseDownRelY = (ed->mouse.down.y - ed->view.y) / ed->view.scale;

			if (ed->tool.type.current == TOOL_LINE) {
				dirty = plotLine((Vec2){ MouseDownRelX, MouseDownRelY }, (Vec2){ MouseRelX, MouseRelY }, &ed->canvas.image, ed->tool.brush.color);
			} else if (ed->tool.type.current == TOOL_RECT) {
				dirty = plotRect((Vec2){ MouseDownRelX, MouseDownRelY }, (Vec2){ MouseRelX, MouseRelY }, &ed->canvas.image, ed->tool.brush.color);
			} else {
				dirty = plotEllipseRect((Vec2){ MouseDownRelX, MouseDownRelY }, (Vec2){ MouseRelX, MouseRelY }, &ed->canvas.image, ed->tool.brush.color);
			}
			break;
		}
	}

	ed->mouse.down.x = ed->mouse.down.y = ed->mouse.last.x = ed->mouse.last.y = 0;

	return dirty;
}

void editor_update_view(Editor* ed) {
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

int editor_set_filepath(Editor* ed, const char* filePath);

void editor_zoom_out(Editor* ed) {
	if (ed->view.scale > 1) {
		ed->view.scale -= 0.15;
	} else {
		ed->view.scale -= 0.05;
	}

	ed->view.scale = ed->view.scale <= 0.05 ? 0.05 : ed->view.scale;
	editor_update_view(ed);
}

void editor_zoom_in(Editor* ed) {
	if (ed->view.scale > 1) {
		ed->view.scale += 0.15;
	} else {
		ed->view.scale += 0.05;
	}

	editor_update_view(ed);
}

void editor_center_view(Editor* ed, Vec2 boundingRect) {
	ed->view.x = ((float)boundingRect.x / 2) - (ed->view.w / 2);
	ed->view.y = ((float)boundingRect.y / 2) - (ed->view.h / 2);
}

void editor_process_input(Editor* ed) {
	ImGuiIO* io = igGetIO();

	if (!igIsMouseDown_Nil(ImGuiMouseButton_Left)) {
		if (io->MouseWheel > 0) editor_zoom_in(ed);
		else if (io->MouseWheel < 0) editor_zoom_out(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiMod_Ctrl | ImGuiKey_Equal)) editor_zoom_in(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiMod_Ctrl | ImGuiKey_Minus)) editor_zoom_out(ed);
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
		dirty = editor_on_mouse_down(ed, io->MousePos.x, io->MousePos.y);
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
		dirty = editor_on_mouse_move(ed, io->MousePos.x, io->MousePos.y);
	}
	if (igIsMouseDragging(ImGuiMouseButton_Left, -1)) {
		editor_on_mouse_drag(ed, io->MousePos.x, io->MousePos.y);
	}
	if (igIsMouseReleased_Nil(ImGuiMouseButton_Left)) {
		dirty = editor_on_mouse_up(ed, io->MousePos.x, io->MousePos.y);
	}

	if (rect_is_valid(&dirty)) {
		texture_update(
			ed->canvas.texture, dirty.start.x, dirty.start.y,
			dirty.end.x - dirty.start.x, dirty.end.y - dirty.start.y,
			ed->canvas.image.width, (unsigned char*)ed->canvas.image.pixels
		);
		rect_invalidate(&dirty);
	}
}
