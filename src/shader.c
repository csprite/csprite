#include "glad/glad.h"
#include "assets.h"
#include "shader.h"

// Create a shader program with given shaders and return a ID, if null, uses the shader.vs & shader.fs file's content hard coded in the source code
unsigned int CreateShaderProgram(const char* vertPath, const char* fragPath, const char* geomPath) {
	unsigned int shader_program;
	unsigned int vert_shader;
	unsigned int frag_shader;
	unsigned int geom_shader;
	const char* vert_shader_source;
	const char* frag_shader_source;
	const char* geom_shader_source;
	int success;
	char info_log[1024];

	if (vertPath != NULL) {
		vert_shader_source = (const char*) assets_get(vertPath, NULL);
		if (vert_shader_source == NULL) {
			printf("cannot load the vertex shader from assets manager.");
			return -1;
		}
	}

	vert_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vert_shader, 1, (const char * const*)&vert_shader_source, NULL);
	glCompileShader(vert_shader);

	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vert_shader, 1024, NULL, info_log);
		printf("Error compiling vertex shader\n%s\n", info_log);
		return -1;
	}

	if (fragPath != NULL) {
		frag_shader_source = (const char*) assets_get(fragPath, NULL);
		if (frag_shader_source == NULL) {
			printf("cannot load the fragment shader from assets manager.");
			return -1;
		}
	}

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, (const char* const*)&frag_shader_source, NULL);
	glCompileShader(frag_shader);

	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(frag_shader, 1024, NULL, info_log);
		printf("Error compiling fragment shader\n%s\n", info_log);
		return -1;
	}

	if (geomPath != NULL) {
		geom_shader_source = (const char*) assets_get(geomPath, NULL);
		if (geom_shader_source == NULL) {
			printf("cannot load the geometry shader from assets manager.");
			return -1;
		}

		geom_shader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geom_shader, 1, (const char* const*)&geom_shader_source, NULL);
		glCompileShader(geom_shader);

		glGetShaderiv(geom_shader, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(geom_shader, 1024, NULL, info_log);
			printf("Error compiling geometry shader\n%s\n", info_log);
		}
	}

	shader_program = glCreateProgram();

	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	if (geomPath != NULL)
		glAttachShader(shader_program, geom_shader);

	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(shader_program, 1024, NULL, info_log);
		printf("Error linking shader \n%s\n", info_log);
		return -1;
	}

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	if (geomPath != NULL)
		glDeleteShader(geom_shader);

	return shader_program;
}
