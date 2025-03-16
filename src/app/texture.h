#ifndef CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#define CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#pragma once

#include "base/types.h"

typedef U32 Texture;

Texture Texture_Init(S32 width, S32 height);
void Texture_Update(Texture id, S32 subreg_x, S32 subreg_y, S32 subreg_w, S32 subreg_h, S32 tWidth, U8* data);
void Texture_Deinit(Texture id);

#endif
