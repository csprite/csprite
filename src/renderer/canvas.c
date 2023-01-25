#include "./canvas.h"
#include "./ogl_wrapper.h"
#include "../logger.h"
#include "../assets.h"

// Static For No Name Collisions
static int32_t CanvasDims[2] = { 0, 0 };
static GLuint vbo = 0; // Stores The Vertices.
static GLuint ebo = 0; // Stores The Indices.
static GLuint vao = 0; // Stores Pointer To Our Vertices And Indices.
static GLuint fboId = 0;
static GLuint fboTexId = 0;
static GLuint textureShaderId = 0;
static GLuint bgTexture = 0;
static uint8_t bgData[2 * 2 * 4] = {
	// 2x2 Pixel Array To Create Checkerboard Background
	0x80, 0x80, 0x80, 0xFF,
	0xC0, 0xC0, 0xC0, 0xFF,
	0xC0, 0xC0, 0xC0, 0xFF,
	0x80, 0x80, 0x80, 0xFF
};

static GLfloat QuadVertices[] = {
//    X      Y      Z      R     G     B      U     V
	 1.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
	 1.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
	-1.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	-1.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f
};
static GLuint QuadIndices[] = { 0, 1, 3, 1, 2, 3 };

GLuint CreateTextureShader();

int Canvas_Init(int32_t w, int32_t h) {
	CanvasDims[0] = w;
	CanvasDims[1] = h;

	textureShaderId = CreateTextureShader();
	GenerateBuffers(&vao, &vbo, &ebo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadIndices), QuadIndices, GL_STATIC_DRAW);

	// Tell OpenGL To Treat The First 3 Vertice As "position" (Used In Vertex Shader)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0); // Make This Data Available At Location 0

	// Tell OpenGL To Treat The Next 3 Vertice As "color" (Used In Fragment Shader)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1); // Make This Data Available At Location 1

	// Tell OpenGL To Treat The Next 2 Vertice As "tex_coords" or "UV" (Used In Fragment Shader)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
	glEnableVertexAttribArray(2); // Make This Data Available At Location 2

	bgTexture = CreateCanvasTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, bgData, 2, 2);
	glBindTexture(GL_TEXTURE_2D, bgTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*
		- VBOs can be unbinded now.
		- EBOs need to be binded as they're used in the main loop for rendering the texture quad
		- VAOs can be unbinded but to reduce glBindVertexArray() calls in the while loop i left it binded
	*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	fboTexId = CreateCanvasTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, NULL, CanvasDims[0], CanvasDims[1]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexId, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Logger_Error("Framebuffer Error: %d\n", fboStatus);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void Canvas_Destroy(void) {
	// Delete All The Buffers, Textures & Shader Programs
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
	glDeleteFramebuffers(1, &fboId);
	DestroyCanvasTexture(&fboTexId);
	DestroyCanvasTexture(&bgTexture);
	DestroyShaderProgram(textureShaderId);

	vbo = ebo = vao = fboId = fboTexId = textureShaderId = bgTexture = CanvasDims[0] = CanvasDims[1] = 0;
}

void Canvas_NewFrame(bool DrawCheckerboardBg) {
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glViewport(0, 0, CanvasDims[0], CanvasDims[1]);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind any texture
	glUseProgram(textureShaderId);   // Use The Texture Shader

	if (DrawCheckerboardBg == true) {
		// Draw The BG Texture Directly Without Uploading The Data Since We've Done It Once Above
		glBindTexture(GL_TEXTURE_2D, bgTexture); // Select Current Texture
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw Texture
	}
}

void Canvas_Resize(int32_t w, int32_t h) {
	CanvasDims[0] = w;
	CanvasDims[1] = h;
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	DestroyCanvasTexture(&fboTexId);
	fboTexId = CreateCanvasTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, NULL, CanvasDims[0], CanvasDims[1]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexId, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, CanvasDims[0], CanvasDims[1]);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Canvas_Layer(CanvasLayer_T* c, bool UpdateTexture) {
	glBindTexture(GL_TEXTURE_2D, c->texture); // Select Current Texture
	if (UpdateTexture == true) {
		// Upload Pixels To Currently Selected Texture On GPU
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CanvasDims[0], CanvasDims[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, c->pixels);
	}
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw Texture
}

void Canvas_Render(GLint ViewportPosX, GLint ViewportPosY, GLsizei ViewportWidth, GLsizei ViewportHeight) {
	glReadBuffer(GL_COLOR_ATTACHMENT0); // Buffer From Which Data will be read to blit
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Buffer From Which Our Canvas Will Be Blitted Onto
	glBlitFramebuffer(
		0, 0, CanvasDims[0], CanvasDims[1], // Our FBO Size
		ViewportPosX, ViewportPosY, ViewportPosX + ViewportWidth, ViewportPosY + ViewportHeight, // Our Output Canvas Size
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint8_t* Canvas_GetRender() {
	uint8_t* data = (uint8_t*) malloc(CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t));
	if (data == NULL) return NULL;
	glBindFramebuffer(GL_FRAMEBUFFER, fboId); // Select Our Canvas Framebuffer
	glReadPixels(0, 0, CanvasDims[0], CanvasDims[1], GL_RGBA, GL_UNSIGNED_BYTE, data); // Read Data From Currently Selected Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return data;
}

GLuint Canvas_GetFBOTex() {
	return fboTexId;
}

CanvasLayer_T* Canvas_CreateLayer() {
	if (CanvasDims[0] == 0 || CanvasDims[1] == 1) return NULL;

	CanvasLayer_T* c = malloc(sizeof(CanvasLayer_T));
	c->pixels = (uint8_t*) malloc(CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t));
	memset(c->pixels, 0, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t));
	c->texture = CreateCanvasTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, c->pixels, CanvasDims[0], CanvasDims[1]);
	c->history = NULL; // Need To Explicitly Set This To NULL Cause SaveHistory Functions Tries To Check If The Pointer Is Not NULL And If So It Tries To Check it's Member.
	SaveHistory(&c->history, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t), c->pixels);
	snprintf(c->name, LAYER_NAME_MAX, "New Layer");

	return c;
}

void Canvas_DestroyLayer(CanvasLayer_T* c) {
	if (c == NULL) return;
	if (c->texture != 0) {
		DestroyCanvasTexture(&c->texture);
		c->texture = 0;
	}
	if (c->pixels != NULL) {
		free(c->pixels);
		c->pixels = NULL;
	}
	FreeHistory(&c->history);
	free(c);
}

GLuint CreateTextureShader() {
	GLuint ShaderId = 0, VertexShaderId = 0, FragmentShaderId = 0;

	int success = 0;
	char info_log[512];

	const char* VertexShaderTxt = (char*)Assets_Get("data/shaders/vertex.glsl", NULL);
	const char* FragmentShaderTxt = (char*)Assets_Get("data/shaders/fragment.glsl", NULL);

	/* START - Load Vertex & Compile Shader From Disk */
	if (VertexShaderTxt == NULL) {
		printf("Error Cannot Read Vertex Shader Source!\n");
		goto onEnd;
	}

	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, (const char* const*)&VertexShaderTxt, NULL);
	glCompileShader(VertexShaderId);
	glGetShaderiv(VertexShaderId, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(VertexShaderId, 512, NULL, info_log);
		printf("Error compiling vertex shader\n%s\n", info_log);
		goto onEnd;
	}
	/* END - Load Vertex & Compile Shader From Disk */

	/* START - Load Fragment & Compile Shader From Disk */
	if (FragmentShaderTxt == NULL) {
		printf("Error Cannot Read Fragment Shader Source\n");
		goto onEnd;
	}

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, (const char* const*)&FragmentShaderTxt, NULL);
	glCompileShader(FragmentShaderId);
	glGetShaderiv(FragmentShaderId, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(FragmentShaderId, 512, NULL, info_log);
		printf("Error compiling fragment shader\n%s\n", info_log);
		goto onEnd;
	}
	/* END - Load Fragment & Compile Shader From Disk */

	// Link All The Compiled Shaders Together
	ShaderId = glCreateProgram();
	if (VertexShaderTxt != NULL) glAttachShader(ShaderId, VertexShaderId);
	if (FragmentShaderTxt != NULL) glAttachShader(ShaderId, FragmentShaderId);

	glLinkProgram(ShaderId);
	glGetProgramiv(ShaderId, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(ShaderId, 512, NULL, info_log);
		printf("Error linking shader \n%s\n", info_log);
		goto onEnd;
	}

onEnd:
	// Free All The Unused Stuff Now
	if (VertexShaderTxt != NULL) glDeleteShader(VertexShaderId);
	if (FragmentShaderTxt != NULL) glDeleteShader(FragmentShaderId);
	VertexShaderTxt = FragmentShaderTxt = NULL;

	return ShaderId;
}
