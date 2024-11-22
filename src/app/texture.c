#include "app/texture.h"
#include "log/log.h"
#include <glad/glad.h>
#include <stdlib.h>

texture_t texture_init(int width, int height) {
	texture_t id = 0;
	glGenTextures(1, &id);

	if (id == 0) {
		log_fatal("Failed to create texture on GPU");
	}

	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* pixels = calloc(width * height, 4);
	if (pixels == NULL) {
		texture_destroy(id);
		log_fatal("Failed to allocate memory");
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(pixels);
	return id;
}

void texture_update(texture_t id, int subreg_x, int subreg_y, int subreg_w, int subreg_h, int tWidth, unsigned char* data) {
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

void texture_destroy(texture_t id) {
	glDeleteTextures(1, &id);
}
