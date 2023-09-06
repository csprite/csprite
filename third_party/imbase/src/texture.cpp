#include "imbase/texture.hpp"
#include "glad/glad.h"

using namespace ImBase;

Texture::Texture(int w, int h, unsigned char* pixels) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	width = w;
	height = h;

	unsigned char* data = pixels;

	if (pixels == nullptr) {
		data = new unsigned char[width * height * 4]();
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	if (pixels == nullptr) {
		delete[] data;
		data = nullptr;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Update(unsigned char* pixels) {
	glBindTexture(GL_TEXTURE_2D, id);
	/* glTexSubImage2D is better to upload the pixels since glTexImage2D basically
	   deletes the buffer on GPU, reallocates it, and copies the data, and in glTexSubImage2D
	   the data is just copied onto the already existing texture. */
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

Texture::~Texture() {
	glDeleteTextures(1, &id);
	id = 0;
}

