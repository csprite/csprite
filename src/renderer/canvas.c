#include <assert.h>
#include "log/log.h"
#include "./canvas.h"
#include "../assets.h"

// Static For No Name Collisions
static int32_t CanvasDims[2] = { 0, 0 };
CanvasLayer_T bgLayer = { "", NULL, NULL, NULL };
SDL_Texture* CanvasTex = NULL;

SDL_Texture* Canvas_GetTex() {
	return CanvasTex;
}

static void _FreeCanvasBgLayer() {
	if (bgLayer.texture) SDL_DestroyTexture(bgLayer.texture);
	if (bgLayer.pixels) free(bgLayer.pixels);
	bgLayer.pixels = NULL;
	bgLayer.texture = NULL;
}

static int _CanvasGenBgLayer(SDL_Renderer* ren) {
	_FreeCanvasBgLayer();
	bgLayer.pixels = (uint8_t*) malloc(CanvasDims[0]/2 * CanvasDims[1]/2 * 4 * sizeof(uint8_t));
	for (int32_t y = 0; y < CanvasDims[1]/2; y++) {
		for (int32_t x = 0; x < CanvasDims[0]/2; x++) {
			uint8_t r = 0xC0, g = 0xC0, b = 0xC0, a = 0xFF;
			if ((x + y) % 2) { r = 0x80; g = 0x80; b = 0x80; a = 0xFF; }
			uint8_t* pixel = &bgLayer.pixels[(y * (int)(CanvasDims[0]/2) + x) * 4];
			*(pixel + 0) = r;
			*(pixel + 1) = g;
			*(pixel + 2) = b;
			*(pixel + 3) = a;
		}
	}
	bgLayer.texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, CanvasDims[0]/2, CanvasDims[1]/2);
	if (bgLayer.texture == NULL) {
		log_error("Cannot create bgLayer, SDL_CreateTexture() returned NULL: %s", SDL_GetError());
		_FreeCanvasBgLayer();
		return EXIT_FAILURE;
	}
	SDL_UpdateTexture(bgLayer.texture, NULL, bgLayer.pixels, CanvasDims[0]/2 * sizeof(uint8_t) * 4);

	if (SDL_SetTextureBlendMode(bgLayer.texture, SDL_BLENDMODE_BLEND) != 0) {
		log_error("SDL_SetTextureBlendMode() returned Non-Zero: %s", SDL_GetError());
		_FreeCanvasBgLayer();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static void _DestroyCanvasTex() {
	if (CanvasTex != NULL) SDL_DestroyTexture(CanvasTex);
	CanvasTex = NULL;
}

static int _InitCanvasTex(SDL_Renderer* ren) {
	CanvasTex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, CanvasDims[0], CanvasDims[1]);
	if (CanvasTex == NULL) {
		log_error("Cannot create CanvasTex, SDL_CreateTexture() returned NULL: %s", SDL_GetError());
		_FreeCanvasBgLayer();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int Canvas_Init(int32_t w, int32_t h, SDL_Renderer* ren) {
	CanvasDims[0] = w;
	CanvasDims[1] = h;

	if (_CanvasGenBgLayer(ren) != EXIT_SUCCESS) {
		_FreeCanvasBgLayer();
	}

	if (_InitCanvasTex(ren) != EXIT_SUCCESS) {
		_FreeCanvasBgLayer();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void Canvas_Destroy(void) {
	_FreeCanvasBgLayer();
	_DestroyCanvasTex();
	CanvasDims[0] = CanvasDims[1] = 0;
}

void Canvas_NewFrame(SDL_Renderer* ren) {
	SDL_SetRenderTarget(ren, CanvasTex);
	SDL_RenderCopy(ren, bgLayer.texture, NULL, NULL);
}

void Canvas_Resize(int32_t w, int32_t h, SDL_Renderer* ren) {
	CanvasDims[0] = w;
	CanvasDims[1] = h;

	_DestroyCanvasTex();
	assert(_InitCanvasTex(ren) == EXIT_SUCCESS);
	assert(_CanvasGenBgLayer(ren) == EXIT_SUCCESS);
}

void Canvas_Layer(CanvasLayer_T* c, bool UpdateTexture, SDL_Renderer* ren) {
	if (UpdateTexture == true) {
		SDL_UpdateTexture(c->texture, NULL, c->pixels, CanvasDims[0] * sizeof(uint8_t) * 4); // Upload Pixels To Currently Selected Texture On GPU
	}
	SDL_RenderCopy(ren, c->texture, NULL, NULL);
}

void Canvas_FrameEnd(SDL_Renderer* ren, SDL_Rect* r) {
	SDL_SetRenderTarget(ren, NULL);
	SDL_RenderCopy(ren, CanvasTex, NULL, r);
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
		log_error("SDL_SetTextureBlendMode() returned Non-Zero: %s", SDL_GetError());
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

CanvasLayerArr_T* Canvas_CreateArr(int32_t capacity) {
	if (capacity <= 0) return NULL;
	CanvasLayerArr_T* arr = malloc(sizeof(CanvasLayerArr_T));
	if (arr == NULL) return NULL;

	arr->size = 0;
	arr->capacity = capacity;
	arr->layers = malloc(sizeof(CanvasLayer_T*) * capacity);
	if (arr->layers == NULL) {
		free(arr);
		return NULL;
	}
	memset(arr->layers, 0, sizeof(CanvasLayer_T*) * capacity);

	return arr;
}

void Canvas_ResizeArr(CanvasLayerArr_T* arr, int32_t newCapacity) {
	if (arr == NULL || arr->capacity == newCapacity || newCapacity == 0) return;
	if (arr->layers == NULL) {
		arr->capacity = newCapacity;
		arr->layers = malloc(sizeof(CanvasLayer_T*) * newCapacity);
		memset(arr->layers, 0, sizeof(CanvasLayer_T*) * newCapacity);
	} else if (newCapacity > arr->capacity) {
		CanvasLayer_T** layers = malloc(sizeof(CanvasLayer_T*) * newCapacity);
		memset(layers, 0, sizeof(CanvasLayer_T*) * newCapacity);
		memcpy(layers, arr->layers, arr->capacity * sizeof(CanvasLayer_T*));
		free(arr->layers);
		arr->layers = layers;
		arr->capacity = newCapacity;
	} else if (arr->capacity > newCapacity) {
		CanvasLayer_T** layers = malloc(sizeof(CanvasLayer_T*) * newCapacity);
		memset(layers, 0, sizeof(CanvasLayer_T*) * newCapacity);
		memcpy(layers, arr->layers, newCapacity * sizeof(CanvasLayer_T*));

		for (int32_t i = newCapacity - 1; i < arr->capacity; ++i) {
			if (arr->layers[i] != NULL) Canvas_DestroyLayer(arr->layers[i]);
		}
		free(arr->layers);
		arr->layers = layers;
		arr->capacity = newCapacity;
	}
}

void Canvas_DestroyArr(CanvasLayerArr_T* arr) {
	if (arr == NULL) return;
	if (arr->layers != NULL) {
		for (int32_t i = 0; i < arr->capacity; ++i) {
			if (arr->layers[i] != NULL) {
				Canvas_DestroyLayer(arr->layers[i]);
				arr->layers[i] = NULL;
			}
		}
		free(arr->layers);
	}
	free(arr);
}

