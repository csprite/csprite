#ifndef SHADER_H
#define SHADER_H

#include "../include/glad/glad.h"
#include <stdio.h>
#include <stdlib.h>

// Read files into buffer
int read_file_into_buffer(char **buffer, const char *path) {
	int fsize;
	char *str; 
	FILE *file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	str = (char *)malloc(fsize + 1);
	fread(str, 1, fsize, file);
	fclose(file);
	str[fsize] = 0;

	*buffer = str;
	return 0;
}

// Create a shader program with given shaders and return a ID, if null, uses the shader.vs & shader.fs file's content hard coded in the source code
unsigned int create_shader_program(const char *vert_path, const char *frag_path, const char *geom_path) {
	unsigned int shader_program;
	unsigned int vert_shader;
	unsigned int frag_shader;
	unsigned int geom_shader;
	char *vert_shader_source;
	char *frag_shader_source;
	char *geom_shader_source;
	int success;
	char info_log[512];

	// This is shader.vs file.
	const char *VertexShaderSourceCode = "#version 330 core\nlayout (location = 0) in vec3 position;\nlayout (location = 1) in vec3 color;\nlayout (location = 2) in vec2 tex_coords;\n\nout vec3 out_color;\nout vec2 out_tex_coords;\n\nvoid main() {\n    gl_Position = vec4(position, 1.0);\n    out_color = color;\n    out_tex_coords = tex_coords;\n}";
	// This is shader.fs file
	const char *FragmentShaderSourceCode = "#version 330 core\nout vec4 frag_color;\n\nin vec3 out_color;\nin vec2 out_tex_coords;\n\nuniform sampler2D a_texture;\nuniform vec2 offset;\nuniform float alpha;\n\nvoid main() {\n    frag_color = vec4(out_color, alpha) * texture(a_texture, out_tex_coords + offset);\n}";

	if (vert_path != NULL)
		read_file_into_buffer(&vert_shader_source, vert_path);
	vert_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vert_shader, 1, (const char * const*)&VertexShaderSourceCode, NULL);
	glCompileShader(vert_shader);

	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
		printf("Error compiling vertex shader\n%s\n", info_log);
		return -1;
	}

	if (frag_path != NULL)
		read_file_into_buffer(&frag_shader_source, frag_path);
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, (const char*const*)&FragmentShaderSourceCode, NULL);
	glCompileShader(frag_shader);

	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
		printf(
			"Error compiling fragment shader\n%s\n",
			info_log);
		return -1;
	}

	if (geom_path != NULL) {
		read_file_into_buffer(&geom_shader_source, geom_path);
		geom_shader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geom_shader, 1, (const char*const*)&geom_shader_source, NULL);
		glCompileShader(geom_shader);

		glGetShaderiv(geom_shader, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(geom_shader, 512, NULL, info_log);
			printf("Error compiling geometry shader\n%s\n", info_log);
		}
	}

	shader_program = glCreateProgram();

	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	if (geom_path != NULL)
		glAttachShader(shader_program, geom_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(
			shader_program,
			512,
			NULL,
			info_log);
		printf("Error linking shader \n%s\n", info_log);
		return -1;
	}

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	if (geom_path != NULL)
		glDeleteShader(geom_shader);

	return shader_program;
}

#endif
