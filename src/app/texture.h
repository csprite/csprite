#ifndef CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#define CSPRITE_SRC_APP_TEXTURE_H_INCLUDED_
#pragma once

typedef unsigned int texture_t;

texture_t TextureInit(int width, int height); // Zero = Error
void TextureUpdate(texture_t id, int tWidth, int tHeight, unsigned char* data);
void TextureDestroy(texture_t id);

#endif
