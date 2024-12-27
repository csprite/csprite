#ifndef CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#define CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#pragma once

typedef unsigned int Texture;

Texture texture_init(int width, int height);
void texture_update(Texture id, int subreg_x, int subreg_y, int subreg_w, int subreg_h, int tWidth, unsigned char* data);
void texture_deinit(Texture id);

#endif
