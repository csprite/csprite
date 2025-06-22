#ifndef SRC_RENDER_RENDER_H
#define SRC_RENDER_RENDER_H 1
#pragma once

#include "base/types.h"
#include "base/arena.h"
#include "os/gfx.h"

typedef struct {
	U64 value;
} R_Handle;

void r_init(OS_Handle window);
void r_swap(OS_Handle window);
void r_release(OS_Handle window);

// Textures
R_Handle r_tex_init(ArenaTemp* a, U64 width, U64 height);
void     r_tex_update(R_Handle texture, S32 subreg_x, S32 subreg_y, S32 subreg_w, S32 subreg_h, S32 tWidth, U8* data);
void     r_tex_release(R_Handle texture);
#define  r_tex_to_ImTextureID(handle) ((ImTextureID)(uintptr_t)(handle.value))

#endif // SRC_RENDER_RENDER_H
