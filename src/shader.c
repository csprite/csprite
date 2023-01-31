#include "glad/glad.h"
#include "assets.h"
#include "shader.h"

// Creates A Simple Texture Shader
unsigned int CreateShaderProgram() {
	unsigned int ShaderId;
	unsigned int VertShaderId;
	unsigned int FragShaderId;
	int success;
	char info_log[1024];

	const char* FragShaderSrc =
		"#version 130\n"
		"out lowp vec4 frag_color;\n"
		"in lowp vec3 out_color;\n"
		"in mediump vec2 out_tex_coords;\n"
		"uniform sampler2D a_texture;\n"
		"uniform mediump vec2 offset;\n"
		"uniform lowp float alpha;\n"
		"void main() {\n"
		"	frag_color = vec4(out_color, alpha) * texture(a_texture, out_tex_coords + offset);\n"
		"}";

	const char* VertexShaderSrc =
		"#version 130\n"
		"in vec3 position;\n"
		"in vec3 color;\n"
		"in vec2 tex_coords;\n"
		"out vec3 out_color;\n"
		"out vec2 out_tex_coords;\n"
		"void main() {\n"
		"	gl_Position = vec4(position, 1.0);\n"
		"	out_color = color;\n"
		"	out_tex_coords = tex_coords;\n"
		"}";

	VertShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertShaderId, 1, (const char * const*)&VertexShaderSrc, NULL);
	glCompileShader(VertShaderId);
	glGetShaderiv(VertShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(VertShaderId, 1024, NULL, info_log);
		printf("Error compiling vertex shader\n%s\n", info_log);
		return -1;
	}

	FragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragShaderId, 1, (const char* const*)&FragShaderSrc, NULL);
	glCompileShader(FragShaderId);
	glGetShaderiv(FragShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(FragShaderId, 1024, NULL, info_log);
		printf("Error compiling fragment shader\n%s\n", info_log);
		return -1;
	}

	ShaderId = glCreateProgram();
	glAttachShader(ShaderId, VertShaderId);
	glAttachShader(ShaderId, FragShaderId);
	glLinkProgram(ShaderId);
	glGetProgramiv(ShaderId, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(ShaderId, 1024, NULL, info_log);
		printf("Error linking shader \n%s\n", info_log);
		return -1;
	}

	glDeleteShader(VertShaderId);
	glDeleteShader(FragShaderId);

	return ShaderId;
}

