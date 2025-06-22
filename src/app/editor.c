#include "app/editor.h"
#include "gfx/gfx.h"
#include "imgui.h"

Editor Editor_Init(U32 width, U32 height) {
	Editor ed = {0};
	Arena a = arena_init();

	{
		ArenaTemp t = arena_begin_temp(&a);
		Rect checkerDim = { width / 2, height / 2 };

		// TODO(pegvin) - Look into PBOs & See if the overhead of uploading data to GPU can be reduced
		ed.canvas.texture = r_tex_init(&t, width, height);
		ed.canvas.checker = r_tex_init(&t, checkerDim.w, checkerDim.h);

		const Pixel pCol1 = { 0xB8, 0xB8, 0xB8, 0xFF }, pCol2 = { 0x74, 0x74, 0x74, 0xFF };
		Pixel* pixels = arena_alloc(t.arena, checkerDim.w * checkerDim.h * sizeof(Pixel));
		for (S64 y = 0; y < checkerDim.h; y++) {
			for (S64 x = 0; x < checkerDim.w; x++) {
				pixels[(y * checkerDim.w) + x] = ((x + y) % 2) ? pCol2 : pCol1;
			}
		}

		r_tex_update(ed.canvas.checker, 0, 0, checkerDim.w, checkerDim.h, checkerDim.w, (unsigned char*)pixels);
		arena_end_temp(t);
	}

	ed.arena = a;
	ed.file.path = NULL;
	ed.file.name = NULL;
	ed.view.scale = 1.5f;
	ed.tool.type.current = TOOL_BRUSH;
	ed.canvas.image = bitmap_from_null(&a, width, height);
	ed.tool.brush.color = (Pixel){ 255, 255, 255, 255 };
	Editor_UpdateView(&ed);

	return ed;
}

// Editor Editor_InitFrom(Arena* a, const char* filePath) {
// 	Editor ed = {0};
// 	Image img = {0};
// 	if (Image_InitFrom(&img, filePath)) {
// 		return ed;
// 	}

// 	ed = Editor_Init(a, img.width, img.height);
// 	Image_Deinit(&ed.canvas.image);
// 	ed.canvas.image = img;
// 	Texture_Update(ed.canvas.texture, 0, 0, ed.canvas.image.width, ed.canvas.image.height, ed.canvas.image.width, (unsigned char*)ed.canvas.image.pixels);

// 	S32 len = strlen(filePath) + 1;
// 	ed.file.path = Memory_Alloc(len);
// 	strncpy(ed.file.path, filePath, len);

// 	return ed;
// }

void Editor_Deinit(Editor* ed) {
	r_tex_release(ed->canvas.texture);
	r_tex_release(ed->canvas.checker);
	arena_release(&ed->arena);
}

Rng2D Editor_OnMouseDown(Editor* ed, S32 x, S32 y) {
    ed->mouse.down.x = x;
    ed->mouse.down.y = y;
	ed->mouse.last.x = x;
	ed->mouse.last.y = y;

	S32 MouseRelX = (S32)((x - ed->view.x) / ed->view.scale);
	S32 MouseRelY = (S32)((y - ed->view.y) / ed->view.scale);

	Rng2D dirty = {0};
	if (
		MouseRelX < 0 || MouseRelY < 0 ||
		(U64)MouseRelX >= ed->canvas.image.width || (U64)MouseRelY >= ed->canvas.image.height
	) {
		return dirty;
	}

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			Pixel color = ed->tool.type.current == TOOL_BRUSH ? ed->tool.brush.color : (Pixel){0, 0, 0, 0};
			ed->canvas.image.pixels[(MouseRelY * ed->canvas.image.width) + MouseRelX] = color;
			dirty.min.x = MouseRelX;
			dirty.min.y = MouseRelY;
			dirty.max.x = MouseRelX + 1;
			dirty.max.y = MouseRelY + 1;
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

Rng2D Editor_OnMouseMove(Editor* ed, S32 x, S32 y) {
	Rng2D dirty = {0};

	S32 MouseRelX = (S32)((x - ed->view.x) / ed->view.scale);
	S32 MouseRelY = (S32)((y - ed->view.y) / ed->view.scale);

	switch (ed->tool.type.current) {
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			if (
				MouseRelX < 0 || MouseRelY < 0 ||
				(U64)MouseRelX > ed->canvas.image.width || (U64)MouseRelY > ed->canvas.image.height
			) {
				break;
			}

			Pixel color = ed->tool.type.current != TOOL_ERASER ? ed->tool.brush.color : (Pixel){0, 0, 0, 0};
			Rng2D newDirty = plotLine(
				(Point){ (S32)((ed->mouse.last.x - ed->view.x)/ed->view.scale), (S32)((ed->mouse.last.y - ed->view.y)/ed->view.scale) },
				(Point){ MouseRelX, MouseRelY },
				&ed->canvas.image, color
			);

			if (newDirty.min.x < dirty.min.x) dirty.min.x = newDirty.min.x;
			if (newDirty.min.y < dirty.min.y) dirty.min.y = newDirty.min.y;
			if (newDirty.max.x > dirty.max.x) dirty.max.x = newDirty.max.x;
			if (newDirty.max.y > dirty.max.y) dirty.max.y = newDirty.max.y;
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

Rng2D Editor_OnMouseUp(Editor* ed, S32 x, S32 y) {
	Rng2D dirty = {0};

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

void Editor_CenterView(Editor* ed, Rect boundingRect) {
	ed->view.x = ((F32)boundingRect.w / 2) - (ed->view.w / 2);
	ed->view.y = ((F32)boundingRect.h / 2) - (ed->view.h / 2);
}

void Editor_ProcessInput(Editor* ed) {
	ImGuiIO* io = igGetIO_Nil();

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

	Rng2D dirty = {0};

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

	if (rng2d_is_nil(dirty)) {
		r_tex_update(
			ed->canvas.texture, dirty.min.x, dirty.min.y,
			dirty.max.x - dirty.min.x, dirty.max.y - dirty.min.y,
			ed->canvas.image.width, (U8*)ed->canvas.image.pixels
		);
		dirty = rng2d_nil();
	}
}
