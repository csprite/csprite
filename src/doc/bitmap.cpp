#include <SDL_error.h>
#include <SDL_rect.h>
#include <SDL_render.h>

#include "log.hpp"
#include "types.hpp"
#include "doc/bitmap.hpp"
#include "pixel/pixel.hpp"
#include "window/renderer.hpp"
#include "stb/stb_image.h"

Bitmap::Bitmap(fs::path path) {
	if (fs::is_regular_file(path)) {
		i32 w = 0, h = 0, c = 0;
		u8* data = stbi_load(path.c_str(), &w, &h, &c, 4);

		width = static_cast<u16>(w);
		height = static_cast<u16>(h);
		pixels = new Pixel[w * h];
		LOG_I("%s: %dx%d - %d", path.filename().c_str(), width, height, c);

		GenerateTexture();
		UpdateTexture();

		if (data == NULL || w <= 0 || h <= 0) {
			LOG_E("Failed to open image: %s", path.c_str());
			return;
		}

		for (u16 y = 0; y < height; ++y) {
			for (u16 x = 0; x < width; ++x) {
				u8* srcPixel = data + (((y * w) + x) * 4);
				Pixel& destPixel = *(pixels + (y * w) + x);
				destPixel.r = *(srcPixel + 0);
				destPixel.g = *(srcPixel + 1);
				destPixel.b = *(srcPixel + 2);
				destPixel.a = *(srcPixel + 3);
			}
		}

		UpdateTexture();
		stbi_image_free(data);
	} else {
		LOG_E("\"%s\" is not a read-able file", path.c_str());
		width = 16;
		height = 16;
		pixels = new Pixel[width * height];
		filePath = path;
		bBox = { 0, 0, 16 * 5, 16 * 5 };
		GenerateTexture();
		UpdateTexture();
	}
}

void Bitmap::GenerateTexture() {
	_gpuTex = SDL_CreateTexture(
		Renderer::Get(),
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		width, height
	);
	if (_gpuTex == NULL) {
		LOG_E("Failed to generate a SDL_Texture: %s", SDL_GetError());
	}
}

void Bitmap::UpdateTexture() {
	SDL_UpdateTexture(_gpuTex, NULL, pixels, width * 4);
}

Bitmap::Bitmap(u16 w, u16 h) {
	width = w;
	height = h;
	pixels = new Pixel[width * height];
	GenerateTexture();
}

Bitmap::~Bitmap() {
	delete[] pixels;
	SDL_DestroyTexture(_gpuTex);
}

void Bitmap::SetBoundingBox(const Rect& newBox) {
	bBox = newBox;
	scaleX = bBox.w / width;
	scaleY = bBox.h / height;
}

void Bitmap::Draw(const Rect &dirtyArea) {
	const SDL_Rect src = static_cast<SDL_Rect>(dirtyArea);
	const SDL_Rect dest = {
		(i32)(bBox.x + (dirtyArea.x * scaleX)),
		(i32)(bBox.y + (dirtyArea.y * scaleY)),
		(i32)(dirtyArea.w * scaleX),
		(i32)(dirtyArea.h * scaleY)
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
