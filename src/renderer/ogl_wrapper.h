#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

GLuint CreateCanvasTexture(
	GLint internalFormat, // Format In Which The Texture Will Be Stored In GPU, Like: GL_RGBA
	GLenum format,        // Format In Which The "data" is In, Like:  GL_RGBA
	GLenum type,          // How The Pixels Are Layed Out In Memory, Like: GL_UNSIGNED_BYTE
	const void* data,     // Pointer To The Texture Data
	unsigned int w,       // Width Of The Texture
	unsigned int h        // Height Of The Texture
);

void DestroyCanvasTexture(GLuint* texture /* Pointer To The Texture ID To Delete */);
void DestroyShaderProgram(GLuint program /* ID Of Shader Program To Delete */);
void GenerateBuffers(
	/*
		Pointer To Variable Where The ID Will Be Stored,
		Any Or All Pointers Can be NULL
	*/
	GLuint* vao,
	GLuint* vbo,
	GLuint* ebo
);

#ifdef __cplusplus
}
#endif
