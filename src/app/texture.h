#ifndef CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#define CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#pragma once

typedef unsigned int texture_t;

texture_t texture_init(int width, int height);
void texture_update(texture_t id, int subreg_x, int subreg_y, int subreg_w, int subreg_h, int tWidth, unsigned char* data);
void texture_deinit(texture_t id);

#endif
