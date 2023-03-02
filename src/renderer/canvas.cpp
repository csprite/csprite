#include "canvas.h"
#include "log/log.h"

CanvasLayer::CanvasLayer(SDL_Renderer* ren, int32_t w, int32_t h, std::string name) {
	this->name = name;
	this->pixels = new uint8_t[w * h * 4]();
	this->tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h);
	this->history = NULL;
	SaveHistory(&this->history, w * h * 4, this->pixels);

	if (SDL_SetTextureBlendMode(this->tex, SDL_BLENDMODE_BLEND) != 0) {
		log_error("SDL_SetTextureBlendMode() returned Non-Zero: %s", SDL_GetError());
	}
}

CanvasLayer::~CanvasLayer() {
	delete[] this->pixels;
	SDL_DestroyTexture(this->tex);
	FreeHistory(&this->history);
}

CanvasLayer_Manager::CanvasLayer_Manager(SDL_Renderer* ren, int32_t w, int32_t h) {
	this->dims[0] = w;
	this->dims[1] = h;
	this->ren = ren;
	this->layers.reserve(50);
	this->layer = NULL;
	this->CurrentLayerIdx = 0;

	this->render = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);
	if (this->render == NULL) {
		log_error("Cannot create renderTex, SDL_CreateTexture() returned NULL: %s", SDL_GetError());
	}

	uint8_t* pixels = new uint8_t[(w/2) * (h/2) * 4];
	for (int32_t y = 0; y < h/2; y++) {
		for (int32_t x = 0; x < w/2; x++) {
			uint8_t r = 0xC0, g = 0xC0, b = 0xC0, a = 0xFF;
			if ((x + y) % 2) { r = 0x80; g = 0x80; b = 0x80; a = 0xFF; }
			uint8_t* pixel = &pixels[(y * (int)(w/2) + x) * 4];
			*(pixel + 0) = r;
			*(pixel + 1) = g;
			*(pixel + 2) = b;
			*(pixel + 3) = a;
		}
	}
	this->pattern = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w/2, h/2);
	if (this->pattern == NULL) {
		log_error("Cannot create pattern, SDL_CreateTexture() returned NULL: %s", SDL_GetError());
	}
	SDL_UpdateTexture(this->pattern, NULL, pixels, (w/2) * 4);

	if (SDL_SetTextureBlendMode(this->pattern, SDL_BLENDMODE_BLEND) != 0) {
		log_error("SDL_SetTextureBlendMode() returned Non-Zero: %s", SDL_GetError());
	}

	delete[] pixels;
}

CanvasLayer_Manager::~CanvasLayer_Manager() {
	for (auto layer_ptr : this->layers) delete layer_ptr;
	SDL_DestroyTexture(this->render);
	SDL_DestroyTexture(this->pattern);
}

void CanvasLayer_Manager::AddLayer(std::string name) {
	auto layer_ptr = new CanvasLayer(this->ren, this->dims[0], this->dims[1], name);
	this->layers.push_back(layer_ptr);
}

void CanvasLayer_Manager::RemoveLayer(int32_t idx) {
	if (idx < this->layers.size()) {
		auto layer_ptr = this->layers[idx];
		this->layers.erase(this->layers.begin() + idx);
		delete layer_ptr;
	}
	if (this->layers.size() == 0) {
		this->layer = NULL;
	}
}

void CanvasLayer_Manager::SetCurrentLayerIdx(int32_t idx) {
	if (this->layers.size() > 0) {
		CurrentLayerIdx = idx;
		this->layer = layers[idx];
	}
}

void CanvasLayer_Manager::ReUploadTexture(int32_t idx) {
	if (this->layers.size() > 0 && idx < this->layers.size()) {
		SDL_UpdateTexture(this->layers[idx]->tex, NULL, this->layers[idx]->pixels, this->dims[0] * 4);
	}
}

void CanvasLayer_Manager::Draw(SDL_Rect* r, int32_t layerToUpdateIdx) {
	SDL_SetRenderTarget(this->ren, this->render);
	SDL_RenderCopy(this->ren, this->pattern, NULL, NULL);

	for (int32_t i = 0; i < this->layers.size(); ++i) {
		if (layerToUpdateIdx == i) {
			SDL_UpdateTexture(this->layers[i]->tex, NULL, this->layers[i]->pixels, this->dims[0] * 4);
		}
		SDL_RenderCopy(this->ren, this->layers[i]->tex, NULL, NULL);
	}

	SDL_SetRenderTarget(this->ren, NULL);
	SDL_RenderCopy(this->ren, this->render, NULL, r);
}

