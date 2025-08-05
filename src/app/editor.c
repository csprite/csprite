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
		ed.image_tex = r_tex_init(&t, width, height);
		ed.checker_tex = r_tex_init(&t, checkerDim.w, checkerDim.h);

		const RGBAU8 pCol1 = { 0xB8, 0xB8, 0xB8, 0xFF }, pCol2 = { 0x74, 0x74, 0x74, 0xFF };
		RGBAU8* pixels = arena_alloc(t.arena, checkerDim.w * checkerDim.h * sizeof(RGBAU8));

		#pragma omp parallel for
		for EachIndex(i, (U64)(checkerDim.w * checkerDim.h)) {
			U64 x = i % checkerDim.w;
			U64 y = i / checkerDim.w;
			pixels[i] = (x + y) % 2 ? pCol2 : pCol1;
		}

		r_tex_update(ed.checker_tex, 0, 0, checkerDim.w, checkerDim.h, checkerDim.w, (unsigned char*)pixels);
		arena_end_temp(t);
	}

	ed.arena = a;
	ed.view_scale = 1.5f;
	ed.tool_curr = TOOL_BRUSH;
	ed.image = rs_init(&a, rect(width, height));
	ed.tool_size = 1;
	ed.tool_color = (RGBAU8){ 255, 255, 255, 255 };
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
// 	Image_Deinit(&ed.image);
// 	ed.image = img;
// 	Texture_Update(ed.image_tex, 0, 0, ed.image.width, ed.image.height, ed.image.width, (unsigned char*)ed.image.pixels);

// 	S32 len = strlen(filePath) + 1;
// 	ed.file.path = Memory_Alloc(len);
// 	strncpy(ed.file.path, filePath, len);

// 	return ed;
// }

void Editor_Deinit(Editor* ed) {
	r_tex_release(ed->image_tex);
	r_tex_release(ed->checker_tex);
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
		(S32)((m_pos.x - ed->view_pos.x) / ed->view_scale),
		(S32)((m_pos.y - ed->view_pos.y) / ed->view_scale)
	);
	Vec2S32 rel_last = v2s32(
		(S32)((ed->mouse_last.x - ed->view_pos.x) / ed->view_scale),
		(S32)((ed->mouse_last.y - ed->view_pos.y) / ed->view_scale)
	);
	Vec2S32 rel_down = v2s32(
		(S32)((ed->mouse_down.x - ed->view_pos.x) / ed->view_scale),
		(S32)((ed->mouse_down.y - ed->view_pos.y) / ed->view_scale)
	);
	RGBAU8 color = ed->tool_curr == TOOL_ERASER ? (RGBAU8){0, 0, 0, 0} : ed->tool_color;

	if (evt == EdMouseEvt_Press) {
		ed->mouse_down = m_pos;
		ed->mouse_last = m_pos;

		if (ed->tool_curr == TOOL_BRUSH || ed->tool_curr == TOOL_ERASER) {
			dirty = rs_gfx_draw_circle(&ed->image, color, rel, ed->tool_size, ed->tool_fill);
		}
	} else if (evt == EdMouseEvt_Move) {
		if (ed->tool_curr == TOOL_BRUSH || ed->tool_curr == TOOL_ERASER) {
			if (ed->tool_size < 2) {
				dirty = rs_gfx_draw_line(&ed->image, color, rel_last, rel);
			} else {
				dirty = rs_gfx_draw_circle(&ed->image, color, rel, ed->tool_size, ed->tool_fill);
			}
		} else if (ed->tool_curr == TOOL_PAN) {
			ed->view_pos.x += m_pos.x - ed->mouse_last.x;
			ed->view_pos.y += m_pos.y - ed->mouse_last.y;
		}
	} else if (evt == EdMouseEvt_Release) {
		switch (ed->tool_curr) {
			case TOOL_LINE: {
				dirty = rs_gfx_draw_line(&ed->image, color, rel_down, rel);
				break;
			}
			case TOOL_RECT: {
				dirty = rs_gfx_draw_rect(&ed->image, color, rel_down, rel);
				break;
			}
			case TOOL_ELLIPSE: {
				dirty = rs_gfx_draw_ellipse(&ed->image, color, rel_down, rel);
				break;
			}
			case TOOL_BRUSH:
			case TOOL_ERASER:
			case TOOL_PAN:
			case TOOL_NONE: break;
		}

		ed->mouse_down = v2s32(-1, -1);
		ed->mouse_last = v2s32(-1, -1);
	}

	if (evt == EdMouseEvt_Move) {
		ed->mouse_last = m_pos;
	}

	return dirty;
}

void ed_draw_tool_preview(Editor* ed, Vec2S32 m_pos) {
	S32 MouseRelX = (S32)((m_pos.x - ed->view_pos.x) / ed->view_scale);
	S32 MouseRelY = (S32)((m_pos.y - ed->view_pos.y) / ed->view_scale);

	S32 MouseDownRelX = (ed->mouse_down.x - ed->view_pos.x) / ed->view_scale;
	S32 MouseDownRelY = (ed->mouse_down.y - ed->view_pos.y) / ed->view_scale;

	Vec2S32 TopLeft = { MouseDownRelX, MouseDownRelY };
	Vec2S32 BotRight = { MouseRelX, MouseRelY };

	v2s32_ensure_tl_br(&TopLeft, &BotRight);

	Vec2S32 BotLeft = { TopLeft.x, BotRight.y };
	Vec2S32 TopRight = { BotRight.x, TopLeft.y };

	switch (ed->tool_curr) {
		case TOOL_LINE: {
			if (!igIsMouseDown_Nil(ImGuiMouseButton_Left) || v2_match(ed->mouse_down, v2s32(-1, -1)))
				break;
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (MouseDownRelX * ed->view_scale) + ed->view_pos.x, (MouseDownRelY * ed->view_scale) + ed->view_pos.y },
				(ImVec2){ ((MouseDownRelX + 1) * ed->view_scale) + ed->view_pos.x, ((MouseDownRelY + 1) * ed->view_scale) + ed->view_pos.y },
				*(U32*)&ed->tool_color, 0, 0
			);
			ImDrawList_AddLine(
				igGetForegroundDrawList_Nil(),
				(ImVec2){ ((MouseDownRelX + 0.5) * ed->view_scale) + ed->view_pos.x, ((MouseDownRelY + 0.5) * ed->view_scale) + ed->view_pos.y },
				(ImVec2){ ((MouseRelX + 0.5) * ed->view_scale) + ed->view_pos.x, ((MouseRelY + 0.5) * ed->view_scale) + ed->view_pos.y },
				*(U32*)&ed->tool_color,
				ed->view_scale / 3
			);
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (MouseRelX * ed->view_scale) + ed->view_pos.x, (MouseRelY * ed->view_scale) + ed->view_pos.y },
				(ImVec2){ ((MouseRelX + 1) * ed->view_scale) + ed->view_pos.x, ((MouseRelY + 1) * ed->view_scale) + ed->view_pos.y },
				*(U32*)&ed->tool_color, 0, 0
			);
			break;
		}
		case TOOL_RECT: {
			if (!igIsMouseDown_Nil(ImGuiMouseButton_Left) || v2_match(ed->mouse_down, v2s32(-1, -1)))
				break;
			ImDrawList_AddRectFilled(
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopLeft.x * ed->view_scale) + ed->view_pos.x, (TopLeft.y * ed->view_scale) + ed->view_pos.y },
				(ImVec2){ ((BotRight.x + 1) * ed->view_scale) + ed->view_pos.x, ((BotRight.y + 1) * ed->view_scale) + ed->view_pos.y },
				*(U32*)&ed->tool_color, 0, 0
			);
			break;
		}
		case TOOL_ELLIPSE: {
			if (!igIsMouseDown_Nil(ImGuiMouseButton_Left) || v2_match(ed->mouse_down, v2s32(-1, -1)))
				break;
			ImDrawList_AddRectFilled( // Top Left
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopLeft.x * ed->view_scale) + ed->view_pos.x, (TopLeft.y * ed->view_scale) + ed->view_pos.y },
				(ImVec2){ ((TopLeft.x + 1) * ed->view_scale) + ed->view_pos.x, ((TopLeft.y + 1) * ed->view_scale) + ed->view_pos.y },
				*(U32*)&ed->tool_color, 0, 0
			);
			ImDrawList_AddRectFilled( // Top Right
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (TopRight.x * ed->view_scale) + ed->view_pos.x, (TopRight.y * ed->view_scale) + ed->view_pos.y },
				(ImVec2){ ((TopRight.x + 1) * ed->view_scale) + ed->view_pos.x, ((TopRight.y + 1) * ed->view_scale) + ed->view_pos.y },
				*(U32*)&ed->tool_color, 0, 0
			);
			ImDrawList_AddRectFilled( // Bottom Left
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (BotLeft.x * ed->view_scale) + ed->view_pos.x, (BotLeft.y * ed->view_scale) + ed->view_pos.y },
				(ImVec2){ ((BotLeft.x + 1) * ed->view_scale) + ed->view_pos.x, ((BotLeft.y + 1) * ed->view_scale) + ed->view_pos.y },
				*(U32*)&ed->tool_color, 0, 0
			);
			ImDrawList_AddRectFilled( // Bottom Right
			    igGetForegroundDrawList_Nil(),
				(ImVec2){ (BotRight.x * ed->view_scale) + ed->view_pos.x, (BotRight.y * ed->view_scale) + ed->view_pos.y },
				(ImVec2){ ((BotRight.x + 1) * ed->view_scale) + ed->view_pos.x, ((BotRight.y + 1) * ed->view_scale) + ed->view_pos.y },
				*(U32*)&ed->tool_color, 0, 0
			);
			ImDrawList_AddEllipse(
				igGetForegroundDrawList_Nil(),
				(ImVec2){
					( ( TopLeft.x + 0.5 + ( (F32)(TopRight.x - TopLeft.x) / 2) ) * ed->view_scale ) + ed->view_pos.x,
					( ( TopLeft.y + 0.5 + ( (F32)(BotLeft.y - TopLeft.y) / 2) ) * ed->view_scale ) + ed->view_pos.y,
				},
				(ImVec2){ ((TopRight.x - TopLeft.x) * ed->view_scale)/2, ((TopLeft.y - BotLeft.y) * ed->view_scale)/2 },
				*(U32*)&ed->tool_color, 0, 0, 1
			);
			break;
		}
		case TOOL_NONE: {
			igSetMouseCursor(ImGuiMouseCursor_Arrow);
			break;
		}
		case TOOL_PAN: {
			igSetMouseCursor(ImGuiMouseCursor_Hand);
			break;
		}
		case TOOL_BRUSH:
		case TOOL_ERASER: {
			if (igIsMouseDown_Nil(ImGuiMouseButton_Left)) {
				break;
			} else if (ed->tool_size < 2) {
				ImDrawList_AddRectFilled(
				    igGetForegroundDrawList_Nil(),
					(ImVec2){ (MouseRelX * ed->view_scale) + ed->view_pos.x, (MouseRelY * ed->view_scale) + ed->view_pos.y },
					(ImVec2){ ((MouseRelX + 1) * ed->view_scale) + ed->view_pos.x, ((MouseRelY + 1) * ed->view_scale) + ed->view_pos.y },
					*(U32*)&ed->tool_color, 0, 0
				);
			} else {
				ImVec2 pos = {
					((MouseRelX + 0.5) * ed->view_scale) + ed->view_pos.x,
					((MouseRelY + 0.5) * ed->view_scale) + ed->view_pos.y
				};
				F32 radius = ed->view_scale * ed->tool_size;
				ImU32 color = *(U32*)&ed->tool_color;
				if (ed->tool_fill) {
					ImDrawList_AddCircleFilled(igGetForegroundDrawList_Nil(), pos, radius, color, 0);
				} else {
					ImDrawList_AddCircle(igGetForegroundDrawList_Nil(), pos, radius, color, 0, 1.0);
				}
			}
			break;
		}
	}
}

void Editor_UpdateView(Editor* ed) {
	ed->view_scale = ed->view_scale < 0.01 ? 0.01 : ed->view_scale;

	// Ensures That The viewRect is Centered From The Center
	F32 currX = (ed->view_size.w / 2.0) + ed->view_pos.x;
	F32 currY = (ed->view_size.h / 2.0) + ed->view_pos.y;

	F32 newX = (ed->image.dim.w * ed->view_scale / 2.0) + ed->view_pos.x;
	F32 newY = (ed->image.dim.h * ed->view_scale / 2.0) + ed->view_pos.y;

	ed->view_pos.x -= newX - currX;
	ed->view_pos.y -= newY - currY;

	// Update The Size Of The viewRect
	ed->view_size.w = ed->image.dim.w * ed->view_scale;
	ed->view_size.h = ed->image.dim.h * ed->view_scale;
}

S32 Editor_SetFilepath(Editor* ed, const char* filePath);

void Editor_ZoomOut(Editor* ed) {
	if (ed->view_scale > 1) {
		ed->view_scale -= 0.15;
	} else {
		ed->view_scale -= 0.05;
	}

	ed->view_scale = ed->view_scale <= 0.05 ? 0.05 : ed->view_scale;
	Editor_UpdateView(ed);
}

void Editor_ZoomIn(Editor* ed) {
	if (ed->view_scale > 1) {
		ed->view_scale += 0.15;
	} else {
		ed->view_scale += 0.05;
	}

	Editor_UpdateView(ed);
}

void Editor_CenterView(Editor* ed, Rect boundingRect) {
	ed->view_pos.x = (boundingRect.w / 2.0) - (ed->view_size.w / 2.0);
	ed->view_pos.y = (boundingRect.h / 2.0) - (ed->view_size.h / 2.0);
}

const char* Editor_ToolGetHumanReadable(EdTool t) {
	switch (t) {
		case TOOL_BRUSH:   return "Brush"; break;
		case TOOL_ERASER:  return "Eraser"; break;
		case TOOL_LINE:    return "Line"; break;
		case TOOL_RECT:    return "Rect"; break;
		case TOOL_ELLIPSE: return "Ellipse"; break;
		case TOOL_PAN:     return "Pan"; break;
		case TOOL_NONE:    return "None"; break;
	}
	return "<unknown>";
}

void Editor_ProcessInput(Editor* ed) {
	ImGuiIO* io = igGetIO_Nil();

	// Mouse Has The Precedence Over Other Inputs
	if (!igIsMouseDown_Nil(ImGuiMouseButton_Left)) {
		if (io->MouseWheel > 0) Editor_ZoomIn(ed);
		else if (io->MouseWheel < 0) Editor_ZoomOut(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiMod_Ctrl | ImGuiKey_Equal)) Editor_ZoomIn(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiMod_Ctrl | ImGuiKey_Minus)) Editor_ZoomOut(ed);
		else if (igIsKeyChordPressed_Nil(ImGuiKey_B)) ed->tool_curr = TOOL_BRUSH;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_E)) ed->tool_curr = TOOL_ERASER;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_L)) ed->tool_curr = TOOL_LINE;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_R)) ed->tool_curr = TOOL_RECT;
		else if (igIsKeyChordPressed_Nil(ImGuiKey_C)) ed->tool_curr = TOOL_ELLIPSE;
		else if (igIsKeyPressed_Bool(ImGuiKey_Space, false)) { ed->tool_prev = ed->tool_curr;ed->tool_curr = TOOL_PAN; }
		else if (igIsKeyReleased_Nil(ImGuiKey_Space)) { ed->tool_curr = ed->tool_prev; }
	}

	// Don't Process Mouse Input If Mouse Position Is Invalid
	if (
		!igIsMousePosValid(NULL) ||
		!igIsMousePosValid(&(ImVec2){ .x = ed->mouse_down.x, .y = ed->mouse_down.y }) ||
		!igIsMousePosValid(&(ImVec2){ .x = ed->mouse_last.x, .y = ed->mouse_last.y })
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
			ed->image_tex, dirty.min.x, dirty.min.y, rect.w, rect.h,
			ed->image.dim.w, (U8*)ed->image.data
		);
		dirty = region_nil();
	}
}
