#include "app/render.h"
#include "os/os.h"

#ifdef TARGET_WINDOWS
	#include <GL/GL.h>
#else
	#include "glad/glad.h"
#endif

R_Handle r_tex_init(ArenaTemp* a, U64 width, U64 height) {
	GLuint id = 0;
	glGenTextures(1, &id);

	if (id == 0) {
		os_abort_with_message(1, str8_lit("Failed to create texture"));
	}

	U8* pixels = arena_alloc(a->arena, width * height * 4);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	R_Handle handle = { .value = id };
	return handle;
}

void r_tex_update(R_Handle texture, S32 subreg_x, S32 subreg_y, S32 subreg_w, S32 subreg_h, S32 tWidth, U8* data) {
	GLuint id = texture.value;
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

void r_tex_release(R_Handle texture) {
	GLuint id = texture.value;
	glDeleteTextures(1, &id);
}
