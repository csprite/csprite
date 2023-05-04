#include <SDL_rect.h>
#include <SDL_render.h>

#include "types.hpp"
#include "doc/bitmap.hpp"
#include "pixel/pixel.hpp"
#include "window/renderer.hpp"

Bitmap::Bitmap(u16 w, u16 h) {
	width = w;
	height = h;
	pixels = new Pixel[width * height];
	_gpuTex = SDL_CreateTexture(
		Renderer::Get(),
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		width, height
	);
}

Bitmap::~Bitmap() {
	delete[] pixels;
	SDL_DestroyTexture(_gpuTex);
}

void Bitmap::Draw(Rect &OutputContainer, Rect &dirtyArea) {
	const i32 scaleX = OutputContainer.w / dirtyArea.w;
	const i32 scaleY = OutputContainer.h / dirtyArea.h;
	const SDL_Rect src = static_cast<const SDL_Rect>(dirtyArea);
	const SDL_Rect dest = {
		OutputContainer.x + (dirtyArea.x * scaleX),
		OutputContainer.y + (dirtyArea.y * scaleY),
		dirtyArea.w * scaleX,
		dirtyArea.h * scaleY
	};

	SDL_UpdateTexture(
		_gpuTex,
		&src,
		pixels,
		width * 4
	);
	SDL_RenderCopy(
		Renderer::Get(),
		_gpuTex,
		&src,
		&dest
	);
}

