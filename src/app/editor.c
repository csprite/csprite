#include "app/editor.h"
#include "cimgui.h"

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

mmRect_t plotRect(int x0, int y0, int x1, int y1, image_t* img, pixel_t color) {
	mmRect_t dirty = { img->width, img->height, 0, 0 };

	int t = 0;
	if (x0 > x1) { t = x0; x0 = x1; x1 = t; }
	if (y0 > y1) { t = y0; y0 = y1; y1 = t; }

	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			if (x > -1 && y > -1 && x < img->width && y < img->height) {
				img->pixels[(y * img->width) + x] = color;
			}
		}
	}

	dirty.min_x = x0 < 0 ? 0 : x0;
	dirty.min_y = y0 < 0 ? 0 : y0;
	dirty.max_x = x1 >= img->width ? img->width : x1 + 1;
	dirty.max_y = y1 >= img->height ? img->height : y1 + 1;

	return dirty;
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

	dirty.max_x++;
	dirty.max_y++;
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
		case TOOL_LINE:
		case TOOL_RECT:
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

	int MouseDownRelX = (ed->mouse.down.x - ed->view.x) / ed->view.scale;
	int MouseDownRelY = (ed->mouse.down.y - ed->view.y) / ed->view.scale;

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			if (MouseRelX < 0 || MouseRelY < 0 || MouseRelX >= ed->canvas.image.width || MouseRelY >= ed->canvas.image.height) return dirty;

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
			int TopLeftX = MouseDownRelX, TopLeftY = MouseDownRelY;
			int BotRightX = MouseRelX, BotRightY = MouseRelY;

			int BotLeftX = MouseDownRelX, BotLeftY = MouseRelY;
			int TopRightX = BotRightX, TopRightY = TopLeftY;

			ImDrawList_AddRectFilled( // Top Left
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopLeftX * ed->view.scale) + ed->view.x, (TopLeftY * ed->view.scale) + ed->view.y },
				(ImVec2){ ((TopLeftX + 1) * ed->view.scale) + ed->view.x, ((TopLeftY + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddLine( // Top Left TO Top Right
				igGetForegroundDrawList_Nil(),
				(ImVec2){ ((TopLeftX + 0.5) * ed->view.scale) + ed->view.x, ((TopLeftY + 0.5) * ed->view.scale) + ed->view.y },
				(ImVec2){ ((TopRightX + 0.5) * ed->view.scale) + ed->view.x, ((TopRightY + 0.5) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color,
				ed->view.scale / 3
			);
			ImDrawList_AddLine( // Top Left TO Bottom Left
				igGetForegroundDrawList_Nil(),
				(ImVec2){ ((TopLeftX + 0.5) * ed->view.scale) + ed->view.x, ((TopLeftY + 0.5) * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotLeftX + 0.5) * ed->view.scale) + ed->view.x, ((BotLeftY + 0.5) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color,
				ed->view.scale / 3
			);
			ImDrawList_AddRectFilled( // Top Right
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopRightX * ed->view.scale) + ed->view.x, (TopRightY * ed->view.scale) + ed->view.y },
				(ImVec2){ ((TopRightX + 1) * ed->view.scale) + ed->view.x, ((TopRightY + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);

			ImDrawList_AddRectFilled( // Bottom Left
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (BotLeftX * ed->view.scale) + ed->view.x, (BotLeftY * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotLeftX + 1) * ed->view.scale) + ed->view.x, ((BotLeftY + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);
			ImDrawList_AddLine( // Bottom Left TO Bottom Right
				igGetForegroundDrawList_Nil(),
				(ImVec2){ ((BotLeftX + 0.5) * ed->view.scale) + ed->view.x, ((BotLeftY + 0.5) * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotRightX + 0.5) * ed->view.scale) + ed->view.x, ((BotRightY + 0.5) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color,
				ed->view.scale / 3
			);
			ImDrawList_AddLine( // Bottom Right TO Top Right
				igGetForegroundDrawList_Nil(),
				(ImVec2){ ((BotRightX + 0.5) * ed->view.scale) + ed->view.x, ((BotRightY + 0.5) * ed->view.scale) + ed->view.y },
				(ImVec2){ ((TopRightX + 0.5) * ed->view.scale) + ed->view.x, ((TopRightY + 0.5) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color,
				ed->view.scale / 3
			);
			ImDrawList_AddRectFilled( // Bottom Right
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (BotRightX * ed->view.scale) + ed->view.x, (BotRightY * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotRightX + 1) * ed->view.scale) + ed->view.x, ((BotRightY + 1) * ed->view.scale) + ed->view.y },
				*(uint32_t*)&ed->tool.brush.color, 0, 0
			);
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

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER:
		case TOOL_PAN:
		case TOOL_NONE: {
			break;
		}
		case TOOL_LINE:
		case TOOL_RECT: {
			int32_t MouseRelX = ((x - ed->view.x) / ed->view.scale);
			int32_t MouseRelY = ((y - ed->view.y) / ed->view.scale);
			int32_t MouseDownRelX = (ed->mouse.down.x - ed->view.x) / ed->view.scale;
			int32_t MouseDownRelY = (ed->mouse.down.y - ed->view.y) / ed->view.scale;
			dirty = ed->tool.type.current == TOOL_LINE ?
				plotLine(MouseDownRelX, MouseDownRelY, MouseRelX, MouseRelY, &ed->canvas.image, ed->tool.brush.color) :
				plotRect(MouseDownRelX, MouseDownRelY, MouseRelX, MouseRelY, &ed->canvas.image, ed->tool.brush.color);
			break;
		}
	}

	ed->mouse.down.x = INT_MIN;
	ed->mouse.down.y = INT_MIN;
	ed->mouse.last.x = INT_MIN;
	ed->mouse.last.y = INT_MIN;

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
		if (io->MouseWheel < 0) EditorZoomOut(ed);

		if (igIsKeyPressed_Bool(ImGuiKey_Equal, false)) {
			if (io->KeyCtrl) { EditorZoomIn(ed); }
		} else if (igIsKeyPressed_Bool(ImGuiKey_Minus, false)) {
			if (io->KeyCtrl) { EditorZoomOut(ed); }
		} else if (igIsKeyPressed_Bool(ImGuiKey_B, false)) {
			ed->tool.type.current = TOOL_BRUSH;
		} else if (igIsKeyPressed_Bool(ImGuiKey_E, false)) {
			ed->tool.type.current = TOOL_ERASER;
		} else if (igIsKeyPressed_Bool(ImGuiKey_L, false)) {
			ed->tool.type.current = TOOL_LINE;
		} else if (igIsKeyPressed_Bool(ImGuiKey_R, false)) {
			ed->tool.type.current = TOOL_RECT;
		} else if (igIsKeyPressed_Bool(ImGuiKey_Space, false)) {
			ed->tool.type.previous = ed->tool.type.current;
			ed->tool.type.current = TOOL_PAN;
		} else if (igIsKeyReleased_Nil(ImGuiKey_Space)) {
			ed->tool.type.current = ed->tool.type.previous;
		}
	}

	mmRect_t dirty = {0};

	if (igIsMouseClicked_Bool(ImGuiMouseButton_Left, false)) {
		dirty = EditorOnMouseDown(ed, io->MousePos.x, io->MousePos.y);
	}
	if (igIsMouseDragging(ImGuiMouseButton_Left, -1.0)) {
		dirty = EditorOnMouseMove(ed, io->MousePos.x, io->MousePos.y);
	}
	if (igIsMouseReleased_Nil(ImGuiMouseButton_Left)) {
		dirty = EditorOnMouseUp(ed, io->MousePos.x, io->MousePos.y);
	}

	// Width & Height are set if change occurs
	if (dirty.max_x > 0) {
		TextureUpdate(
			ed->canvas.texture, dirty.min_x, dirty.min_y,
			dirty.max_x - dirty.min_x, dirty.max_y - dirty.min_y,
			ed->canvas.image.width, (unsigned char*)ed->canvas.image.pixels
		);
		dirty.max_x = 0;
	}
}
