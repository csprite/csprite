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
		"out vec4 FinalColor;\n"
		"in vec3 InputColor;\n"
		"in vec2 UV;\n"
		"uniform sampler2D TexSampler;\n"
		"uniform float alpha;\n"
		"void main() {\n"
		"	FinalColor = vec4(InputColor, alpha) * texture(TexSampler, UV);\n"
		"}";

	const char* VertexShaderSrc =
		"#version 130\n"
		"in vec3 InputPos;\n"
		"in vec3 InputColor;\n"
		"in vec2 InputUV;\n"
		"out vec3 OutColor;\n"
		"out vec2 UV;\n"
		"void main() {\n"
		"	gl_Position = vec4(InputPos, 1.0);\n"
		"	OutColor = InputColor;\n"
		"	UV = InputUV;\n"
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

