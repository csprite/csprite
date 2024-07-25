#include "app/texture.h"
#include <glad/glad.h>
#include <stdlib.h>

texture_t TextureInit(int width, int height) {
	texture_t id = 0;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* pixels = calloc(width * height, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(pixels);
	return id;
}

void TextureUpdate(texture_t id, int tWidth, int tHeight, unsigned char* data) {
	glBindTexture(GL_TEXTURE_2D, id);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, tWidth);
	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		0, 0, tWidth, tHeight,
		GL_RGBA, GL_UNSIGNED_BYTE,
		data
	);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureDestroy(texture_t id) {
	glDeleteTextures(1, &id);
}
