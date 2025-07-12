#include "app/editor.h"
#include "raster/gfx.h"
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

		const RGBAU8 pCol1 = { 0xB8, 0xB8, 0xB8, 0xFF }, pCol2 = { 0x74, 0x74, 0x74, 0xFF };
		RGBAU8* pixels = arena_alloc(t.arena, checkerDim.w * checkerDim.h * sizeof(RGBAU8));

		#pragma omp parallel for
		for EachIndex(i, (U64)(checkerDim.w * checkerDim.h)) {
			U64 x = i % checkerDim.w;
			U64 y = i / checkerDim.w;
			pixels[i] = (x + y) % 2 ? pCol2 : pCol1;
		}

		r_tex_update(ed.canvas.checker, 0, 0, checkerDim.w, checkerDim.h, checkerDim.w, (unsigned char*)pixels);
		arena_end_temp(t);
	}

	ed.arena = a;
	ed.file.path = NULL;
	ed.file.name = NULL;
	ed.view.scale = 1.5f;
	ed.tool.type.current = TOOL_BRUSH;
	ed.canvas.image = rs_init(&a, rect(width, height));
	ed.tool.brush.size = 1;
	ed.tool.brush.color = (RGBAU8){ 255, 255, 255, 255 };
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

Region ed_mouse(Editor* ed, EdMouseBtn btn, EdMouseEvt evt, Vec2S32 m_pos) {
	Assert(evt != EdMouseEvt_COUNT);
	Assert(btn != EdMouseBtn_COUNT);

	Region dirty = region_nil();

	// Only consume press/release events of left mouse btn.
	if (btn != EdMouseBtn_Left && evt != EdMouseEvt_Move) {
		return dirty;
	}

	Vec2S32 rel = v2s32(
		(S32)((m_pos.x - ed->view.x) / ed->view.scale),
		(S32)((m_pos.y - ed->view.y) / ed->view.scale)
	);
	Vec2S32 rel_last = v2s32(
		(S32)((ed->mouse.last.x - ed->view.x) / ed->view.scale),
		(S32)((ed->mouse.last.y - ed->view.y) / ed->view.scale)
	);
	Vec2S32 rel_down = v2s32(
		(S32)((ed->mouse.down.x - ed->view.x) / ed->view.scale),
		(S32)((ed->mouse.down.y - ed->view.y) / ed->view.scale)
	);
	RGBAU8 color = ed->tool.type.current == TOOL_ERASER ? (RGBAU8){0, 0, 0, 0} : ed->tool.brush.color;

	if (evt == EdMouseEvt_Press) {
		ed->mouse.down = m_pos;
		ed->mouse.last = m_pos;

		if (ed->tool.type.current == TOOL_BRUSH || ed->tool.type.current == TOOL_ERASER) {
			dirty = rs_gfx_draw_circle(&ed->canvas.image, color, rel, ed->tool.brush.size, ed->tool.brush.filled);
		}
	} else if (evt == EdMouseEvt_Move) {
		if (ed->tool.type.current == TOOL_BRUSH || ed->tool.type.current == TOOL_ERASER) {
			if (ed->tool.brush.size < 2) {
				dirty = rs_gfx_draw_line(&ed->canvas.image, color, rel_last, rel);
			} else {
				dirty = rs_gfx_draw_circle(&ed->canvas.image, color, rel, ed->tool.brush.size, ed->tool.brush.filled);
			}
		} else if (ed->tool.type.current == TOOL_PAN) {
			ed->view.x += m_pos.x - ed->mouse.last.x;
			ed->view.y += m_pos.y - ed->mouse.last.y;
		}
	} else if (evt == EdMouseEvt_Release) {
		switch (ed->tool.type.current) {
			case TOOL_LINE: {
				dirty = rs_gfx_draw_line(&ed->canvas.image, color, rel_down, rel);
				break;
			}
			case TOOL_RECT: {
				dirty = rs_gfx_draw_rect(&ed->canvas.image, color, rel_down, rel);
				break;
			}
			case TOOL_ELLIPSE: {
				dirty = rs_gfx_draw_ellipse(&ed->canvas.image, color, rel_down, rel);
				break;
			}
			case TOOL_BRUSH:
			case TOOL_ERASER:
			case TOOL_PAN:
			case TOOL_NONE: break;
		}

		ed->mouse.down = v2s32(-1, -1);
		ed->mouse.last = v2s32(-1, -1);
	}

	if (evt == EdMouseEvt_Move) {
		ed->mouse.last = m_pos;
	}

	return dirty;
}

void ed_draw_tool_preview(Editor* ed, Vec2S32 m_pos) {
	S32 MouseRelX = (S32)((m_pos.x - ed->view.x) / ed->view.scale);
	S32 MouseRelY = (S32)((m_pos.y - ed->view.y) / ed->view.scale);

	S32 MouseDownRelX = (ed->mouse.down.x - ed->view.x) / ed->view.scale;
	S32 MouseDownRelY = (ed->mouse.down.y - ed->view.y) / ed->view.scale;

	Vec2S32 TopLeft = { MouseDownRelX, MouseDownRelY };
	Vec2S32 BotRight = { MouseRelX, MouseRelY };

	v2s32_ensure_tl_br(&TopLeft, &BotRight);

	Vec2S32 BotLeft = { TopLeft.x, BotRight.y };
	Vec2S32 TopRight = { BotRight.x, TopLeft.y };

	// Draw mouse position regardless
	ImDrawList_AddRect(
	    igGetForegroundDrawList_Nil(),
		(ImVec2){ (MouseRelX * ed->view.scale) + ed->view.x, (MouseRelY * ed->view.scale) + ed->view.y },
		(ImVec2){ ((MouseRelX + 1) * ed->view.scale) + ed->view.x, ((MouseRelY + 1) * ed->view.scale) + ed->view.y },
		*(U32*)&ed->tool.brush.color, 0, 0, 1
	);

	switch (ed->tool.type.current) {
		case TOOL_LINE: {
			if (!igIsMouseDown_Nil(ImGuiMouseButton_Left) || v2_match(ed->mouse.down, v2s32(-1, -1)))
				break;
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
			if (!igIsMouseDown_Nil(ImGuiMouseButton_Left) || v2_match(ed->mouse.down, v2s32(-1, -1)))
				break;
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopLeft.x * ed->view.scale) + ed->view.x, (TopLeft.y * ed->view.scale) + ed->view.y },
				(ImVec2){ ((BotRight.x + 1) * ed->view.scale) + ed->view.x, ((BotRight.y + 1) * ed->view.scale) + ed->view.y },
				*(U32*)&ed->tool.brush.color, 0, 0
			);
			break;
		}
		case TOOL_ELLIPSE: {
			if (!igIsMouseDown_Nil(ImGuiMouseButton_Left) || v2_match(ed->mouse.down, v2s32(-1, -1)))
				break;
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

void Editor_UpdateView(Editor* ed) {
	ed->view.scale = ed->view.scale < 0.01 ? 0.01 : ed->view.scale;

	// Ensures That The viewRect is Centered From The Center
	F32 currX = (ed->view.w / 2) + ed->view.x;
	F32 currY = (ed->view.h / 2) + ed->view.y;

	F32 newX = (ed->canvas.image.dim.w * ed->view.scale / 2) + ed->view.x;
	F32 newY = (ed->canvas.image.dim.h * ed->view.scale / 2) + ed->view.y;

	ed->view.x -= newX - currX;
	ed->view.y -= newY - currY;

	// Update The Size Of The viewRect
	ed->view.w = ed->canvas.image.dim.w * ed->view.scale;
	ed->view.h = ed->canvas.image.dim.h * ed->view.scale;
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

	// Don't Process Mouse Input If Mouse Position Is Invalid
	if (
		!igIsMousePosValid(NULL) ||
		!igIsMousePosValid(&(ImVec2){ .x = ed->mouse.down.x, .y = ed->mouse.down.y }) ||
		!igIsMousePosValid(&(ImVec2){ .x = ed->mouse.last.x, .y = ed->mouse.last.y })
	) {
		return;
	}

	Region dirty = {0};
	Vec2S32 m_pos = v2s32(io->MousePos.x, io->MousePos.y);
	ed_draw_tool_preview(ed, m_pos);

	if (igIsMouseClicked_Bool(ImGuiMouseButton_Left, false)) {
		dirty = ed_mouse(ed, EdMouseBtn_Left, EdMouseEvt_Press, m_pos);
	} else if (igIsMouseDown_Nil(ImGuiMouseButton_Left) && (io->MouseDelta.x != 0 || io->MouseDelta.y != 0)) {
		dirty = ed_mouse(ed, EdMouseBtn_Left, EdMouseEvt_Move, m_pos);
	} else if (igIsMouseReleased_Nil(ImGuiMouseButton_Left)) {
		dirty = ed_mouse(ed, EdMouseBtn_Left, EdMouseEvt_Release, m_pos);
	}

	if (!region_is_nil(dirty)) {
		Rect rect = region_to_rect(dirty);
		r_tex_update(
			ed->canvas.texture, dirty.min.x, dirty.min.y, rect.w, rect.h,
			ed->canvas.image.dim.w, (U8*)ed->canvas.image.data
		);
		dirty = region_nil();
	}
}
