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
