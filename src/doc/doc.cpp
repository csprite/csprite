#include <cstring>
#include "doc/doc.hpp"

DocLayer::~DocLayer() {
	if (pixels != nullptr) {
		delete[] pixels;
		pixels = nullptr;
	}
}

bool Doc::CreateNew(u16 _w, u16 _h, bool _headless) {
	w = _w;
	h = _h;
	headless = _headless;
	finalRender = new Pixel[w * h]{ 0, 0, 0, 0 };
	if (!_headless)
		tex = new ImBase::Texture(_w, _h, nullptr);

	return true;
}

void Doc::AddLayer(const char* name) {
	DocLayer* layer = new DocLayer();
	layer->name = name;
	layer->pixels = new Pixel[w * h]{ 0, 0, 0, 0 };
	layers.push_back(layer);
}

void Doc::RemoveLayer(u16 index) {
	if (index < layers.size()) {
		if (layers[index] != nullptr) {
			delete layers[index];
			layers.erase(layers.begin() + index);
		}
	}
}

Doc::~Doc() {
	for (std::size_t i = 0; i < layers.size(); ++i) {
		if (layers[i] != nullptr) delete layers[i];
	}
	layers.clear();

	if (finalRender != nullptr) {
		delete[] finalRender;
		finalRender = nullptr;
	}

	if (tex != nullptr) {
		delete tex;
		tex = nullptr;
	}
}

void Doc::Render(RectI32& dirtyArea) {
	for (auto y = dirtyArea.y; y < dirtyArea.h; ++y) {
		std::memset(&finalRender[(y * dirtyArea.w) + dirtyArea.x], 0, sizeof(Pixel) * dirtyArea.w);
	}

	for (std::size_t i = 0; i < layers.size(); ++i) {
		for (i32 y = dirtyArea.y; y < dirtyArea.h; ++y) {
			for (i32 x = dirtyArea.x; x < dirtyArea.w; ++x) {
				// Simple Alpha-Blending Being Done Here.
				Pixel& frontPixel = layers[i]->pixels[(y * w) + x];
				Pixel& backPixel = finalRender[(y * w) + x];

				if (frontPixel.a == 255) {
					backPixel = frontPixel;
					continue;
				}

				backPixel.r = CLAMP_NUM_TO_TYPE(
					static_cast<u16>(
						((u16)frontPixel.r * frontPixel.a + (u16)backPixel.r * (255 - frontPixel.a) / 255 * backPixel.a) / 255
					), u8);
				backPixel.g = CLAMP_NUM_TO_TYPE(
					static_cast<u16>(
						((u16)frontPixel.g * frontPixel.a + (u16)backPixel.g * (255 - frontPixel.a) / 255 * backPixel.a) / 255
					), u8);
				backPixel.b = CLAMP_NUM_TO_TYPE(
					static_cast<u16>(
						((u16)frontPixel.b * frontPixel.a + (u16)backPixel.b * (255 - frontPixel.a) / 255 * backPixel.a) / 255
					), u8);
				backPixel.a = CLAMP_NUM_TO_TYPE(
					static_cast<u16>(
						frontPixel.a + (u16)backPixel.a * (255 - frontPixel.a) / 255
					), u8);
			}
		}
	}

	tex->Update((unsigned char*)finalRender);
}
