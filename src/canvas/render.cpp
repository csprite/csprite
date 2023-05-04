#include "canvas/render.hpp"
#include "pixel/pixel.hpp"
#include "window/renderer.hpp"
#include <SDL_rect.h>
#include <SDL_render.h>

Canvas::Render::Render(u16 width, u16 height) {
	texture = SDL_CreateTexture(Renderer::Get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	pixels = new Pixel[width * height];
	pitch = width * 4;
}

Canvas::Render::~Render() {
	SDL_DestroyTexture(texture);
	delete[] pixels;
}

void Canvas::Draw(Render &finalRender, Rect &container, Rect &dirtyArea) {
	const SDL_Rect src = static_cast<const SDL_Rect>(dirtyArea);
	const SDL_Rect dest = static_cast<const SDL_Rect>(container);
	SDL_UpdateTexture(
		finalRender.texture,
		&src,
		finalRender.pixels,
		finalRender.pitch
	);
	SDL_RenderCopy(
		Renderer::Get(),
		finalRender.texture,
		&src,
		&dest
	);
}
