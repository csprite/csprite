#include "app/texture.h"
#include "base/arena.h"
#include "log/log.h"
#include <glad/glad.h>
#include <stdlib.h>

Texture Texture_Init(ArenaTemp* a, S32 width, S32 height) {
	Texture id = 0;
	glGenTextures(1, &id);

	if (id == 0) {
		log_fatal("Failed to create texture on GPU");
	}

	U8* pixels = arena_alloc(a->arena, width * height * 4);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
	return id;
}

void Texture_Update(Texture id, S32 subreg_x, S32 subreg_y, S32 subreg_w, S32 subreg_h, S32 tWidth, U8* data) {
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

void Texture_Deinit(Texture id) {
	glDeleteTextures(1, &id);
}
