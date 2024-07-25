#ifndef CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#define CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#pragma once

unsigned int TextureInit(int width, int height);
void TextureUpdate(unsigned int id, int tWidth, int tHeight, unsigned char* data);
void TextureDestroy(unsigned int id);

#endif
