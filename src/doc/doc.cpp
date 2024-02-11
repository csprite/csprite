#include <cstring>
#include "doc/doc.hpp"

bool Doc::Create(u32 w, u32 h) {
	image.Create(w, h);
	render = new Pixel[w * h]{ 0, 0, 0, 0 };
	renderTex = new ImBase::Texture(w, h, nullptr);

	return true;
}

void Doc::Destroy() {
	image.Destroy();

	delete[] render;
	render = nullptr;

	delete renderTex;
	renderTex = nullptr;
}

