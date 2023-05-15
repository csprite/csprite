#include "renderer/canvas.hpp"

Canvas::Canvas(u16 w, u16 h) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	Pixel* data = new Pixel[w * h]{ 0, 0, 0, 0 };
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, w, h,
		0, GL_RGBA, GL_UNSIGNED_BYTE, data
	);
	delete[] data;
	data = nullptr;

	glBindTexture(GL_TEXTURE_2D, 0);
}

Canvas::~Canvas() {
	glDeleteTextures(1, &id);
	id = 0;
}

void Canvas::Update(Rect& dirtyArea, Pixel* pixelData) {
	glBindTexture(GL_TEXTURE_2D, id);
	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		dirtyArea.x, dirtyArea.y,
		dirtyArea.w, dirtyArea.h,
		GL_RGBA, GL_UNSIGNED_BYTE,
		pixelData
	);
}

namespace CanvasRenderer {
	static Canvas::TextureID TextureShader = 0;
	static u32 vertexBuffObj = 0,
	           vertexArrObj = 0,
               ebo = 0;

	static GLfloat TextureVertices[] = {
		//       Canvas              Color To       Texture
		//     Coordinates          Blend With     Coordinates
		//  X      Y      Z      R     G     B      X     Y
		1.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // Top Right
		1.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Bottom Right
		-1.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, // Bottom Left
		-1.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f  // Top Left
		// Z Coordinates Are 0 Because We Are Working With 2D Stuff
		// Color To Blend With Are The Colors Which Will Be multiplied by the selected color to get the final output on the canvas
	};

	// Index Buffer
	static u32 TextureIndices[] = {0, 1, 3, 1, 2, 3};

	Canvas::TextureID CreateTextureShader();
}

void CanvasRenderer::Init() {
	TextureShader = CanvasRenderer::CreateTextureShader();

	glGenVertexArrays(1, &vertexArrObj);
	glGenBuffers(1, &vertexBuffObj);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vertexArrObj);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TextureVertices), TextureVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TextureIndices), TextureIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glBindAttribLocation(TextureShader, 0, "position");
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glBindAttribLocation(TextureShader, 1, "color");
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glBindAttribLocation(TextureShader, 2, "tex_coords");
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CanvasRenderer::Draw(Canvas& c) {
	glUseProgram(TextureShader);
	glBindVertexArray(vertexArrObj);
	glBindTexture(GL_TEXTURE_2D, 0);

	u32 alpha_loc = glGetUniformLocation(TextureShader, "alpha");
	glUniform1f(alpha_loc, 0.2f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUniform1f(alpha_loc, 1.0f);
	glBindTexture(GL_TEXTURE_2D, c.id);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void CanvasRenderer::Release() {
	glDeleteBuffers(1, &vertexBuffObj);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vertexArrObj);
	glDeleteProgram(TextureShader);

	TextureShader = 0, vertexBuffObj = 0, vertexArrObj = 0, ebo = 0;
}

Canvas::TextureID CanvasRenderer::CreateTextureShader() {
	u32 ShaderId;
	u32 VertShaderId;
	u32 FragShaderId;
	i32 success;
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
