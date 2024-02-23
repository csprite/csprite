#include "image/image.hpp"

bool Layer::Create(u32 w, u32 h) {
	pixels = new Pixel[w * h]{ 0, 0, 0, 0 };
	if (pixels == nullptr) {
		return false;
	}

	return true;
}

void Layer::Destroy() {
	delete[] pixels;
	pixels = nullptr;
	name.clear();
}

bool Image::Create(u32 _w, u32 _h) {
	w = _w;
	h = _h;

	return true;
}

void Image::AddLayer() {
	Layer layer;
	layer.Create(w, h);
	Layers.push_back(layer);
}

void Image::RemoveLayer(u32 index) {
	Layers[index].Destroy();
	Layers.erase(Layers.begin() + index);
}

void Image::Destroy() {
	for (std::size_t i = 0; i < Layers.size(); i++) {
		Layers[i].Destroy();
	}
	Layers.clear();
	w = 0;
	h = 0;
}
