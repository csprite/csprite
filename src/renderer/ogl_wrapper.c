#include "./ogl_wrapper.h"

void GenerateBuffers(GLuint* vao, GLuint* vbo, GLuint* ebo) {
	if (vao) glGenVertexArrays(1, vao);
	if (vbo) glGenBuffers(1, vbo);
	if (ebo) glGenBuffers(1, ebo);
}

GLuint CreateCanvasTexture(GLint internalFormat, GLenum format, GLenum type, const void* data, unsigned int w, unsigned int h) {
	if (w <= 0 || h <= 0) return 0;

	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void DestroyCanvasTexture(GLuint* texture) {
	glDeleteTextures(1, texture);
}

void DestroyShaderProgram(GLuint program) {
	glDeleteProgram(program);
}
