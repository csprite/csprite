#include "./canvas.h"
#include "../logger.h"
#include "../assets.h"

// Static For No Name Collisions
static int32_t CanvasDims[2] = { 0, 0 };
CanvasLayer_T bgLayer = { "", NULL, NULL, NULL };

int Canvas_Init(int32_t w, int32_t h, SDL_Renderer* ren) {
	CanvasDims[0] = w;
	CanvasDims[1] = h;

	bgLayer.pixels = (uint8_t*) malloc(w/2 * h/2 * 4 * sizeof(uint8_t));
	for (int32_t y = 0; y < h/2; y++) {
		for (int32_t x = 0; x < w/2; x++) {
			uint8_t r = 0xC0, g = 0xC0, b = 0xC0, a = 0xFF;
			if ((x + y) % 2) { r = 0x80; g = 0x80; b = 0x80; a = 0xFF; }
			uint8_t* pixel = &bgLayer.pixels[(y * (int)(w/2) + x) * 4];
			*(pixel + 0) = r;
			*(pixel + 1) = g;
			*(pixel + 2) = b;
			*(pixel + 3) = a;
		}
	}
	bgLayer.texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w/2, h/2);
	if (bgLayer.texture == NULL) {
		Logger_Error("Cannot create bgLayer, SDL_CreateTexture() returned NULL: %s", SDL_GetError());
		free(bgLayer.pixels);
		bgLayer.pixels = NULL;
		return EXIT_FAILURE;
	}
	SDL_UpdateTexture(bgLayer.texture, NULL, bgLayer.pixels, w/2 * sizeof(uint8_t) * 4);

	if (SDL_SetTextureBlendMode(bgLayer.texture, SDL_BLENDMODE_BLEND) != 0) {
		Logger_Error("SDL_SetTextureBlendMode() returned Non-Zero: %s", SDL_GetError());
		SDL_DestroyTexture(bgLayer.texture);
		free(bgLayer.pixels);
		bgLayer.pixels = NULL;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void Canvas_Destroy(void) {
	if (bgLayer.pixels != NULL) { free(bgLayer.pixels); bgLayer.pixels = NULL; }
	if (bgLayer.texture != NULL) { SDL_DestroyTexture(bgLayer.texture); bgLayer.texture = NULL; }
	CanvasDims[0] = CanvasDims[1] = 0;
}

void Canvas_NewFrame(bool DrawCheckerboardBg, SDL_Renderer* ren, SDL_Rect* r) {
	if (DrawCheckerboardBg == true) {
		SDL_RenderCopy(ren, bgLayer.texture, NULL, r);
	}
}

void Canvas_Resize(int32_t w, int32_t h) {
	CanvasDims[0] = w;
	CanvasDims[1] = h;
}

void Canvas_Layer(CanvasLayer_T* c, bool UpdateTexture, SDL_Renderer* ren, SDL_Rect* r) {
	if (UpdateTexture == true) {
		SDL_UpdateTexture(c->texture, NULL, c->pixels, CanvasDims[0] * sizeof(uint8_t) * 4); // Upload Pixels To Currently Selected Texture On GPU
	}
	SDL_RenderCopy(ren, c->texture, NULL, r);
}

uint8_t* Canvas_GetRender() {
	uint8_t* data = (uint8_t*) malloc(CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t));
	if (data == NULL) return NULL;
	return data;
}

CanvasLayer_T* Canvas_CreateLayer(SDL_Renderer* ren) {
	if (CanvasDims[0] == 0 || CanvasDims[1] == 1) return NULL;

	CanvasLayer_T* c = malloc(sizeof(CanvasLayer_T));
	c->pixels = (uint8_t*) malloc(CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t));
	memset(c->pixels, 0, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t));
	c->texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, CanvasDims[0], CanvasDims[1]);
	c->history = NULL; // Need To Explicitly Set This To NULL Cause SaveHistory Functions Tries To Check If The Pointer Is Not NULL And If So It Tries To Check it's Member.
	SaveHistory(&c->history, CanvasDims[0] * CanvasDims[1] * 4 * sizeof(uint8_t), c->pixels);
	snprintf(c->name, LAYER_NAME_MAX, "New Layer");

	if (SDL_SetTextureBlendMode(c->texture, SDL_BLENDMODE_BLEND) != 0) {
		Logger_Error("SDL_SetTextureBlendMode() returned Non-Zero: %s", SDL_GetError());
		Canvas_DestroyLayer(c);
		c = NULL;
	}

	return c;
}

void Canvas_DestroyLayer(CanvasLayer_T* c) {
	if (c == NULL) return;
	if (c->texture != 0) {
		SDL_DestroyTexture(c->texture);
		c->texture = NULL;
	}
	if (c->pixels != NULL) {
		free(c->pixels);
		c->pixels = NULL;
	}
	FreeHistory(&c->history);
	free(c);
}


