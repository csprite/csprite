#include "app/texture.h"
#include "base/memory.h"
#include "log/log.h"
#include <glad/glad.h>
#include <stdlib.h>

Texture texture_init(int width, int height) {
	Texture id = 0;
	glGenTextures(1, &id);

	if (id == 0) {
		log_fatal("Failed to create texture on GPU");
	}

	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* pixels = Memory_AllocOrDie(width * height * 4);
	Memory_ZeroAll(pixels, width * height * 4);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
	Memory_Dealloc(pixels);
	return id;
}

void texture_update(Texture id, int subreg_x, int subreg_y, int subreg_w, int subreg_h, int tWidth, unsigned char* data) {
	glBindTexture(GL_TEXTURE_2D, id);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, tWidth);
	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		subreg_x, subreg_y, subreg_w, subreg_h,
		GL_RGBA, GL_UNSIGNED_BYTE,
		&data[((subreg_y * tWidth) + subreg_x) * 4]
	);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_deinit(Texture id) {
	glDeleteTextures(1, &id);
}
