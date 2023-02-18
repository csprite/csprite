#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "../history.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LAYER_NAME_MAX 512

typedef struct {
	char         name[LAYER_NAME_MAX];
	SDL_Texture* texture;
	uint8_t*     pixels;
	History_T*   history;
} CanvasLayer_T;

typedef struct {
	int32_t size; // num of "slots" used
	int32_t capacity; // total num of "slots"
	CanvasLayer_T** layers;
	SDL_Texture* renderTex; // all of the layers will be written on this
	SDL_Texture* patternTex; // bg checkerboard pattern
} CanvasLayerArr_T;

void Canvas_Init(int32_t w, int32_t h); // Initialize Canvas
void Canvas_Destroy(void); // Destroy Canvas
void Canvas_Draw(SDL_Renderer* ren, CanvasLayerArr_T* layers, SDL_Rect* r, int32_t layerToUpdateIndex); // layerToUpdateIndex is a index which is equal to the index of the layer you want to update texture data of
void Canvas_Resize(int32_t w, int32_t h); // Resize Canvas

SDL_Texture*      Canvas_GetTex();
CanvasLayerArr_T* Canvas_CreateArr(int32_t capacity);
void              Canvas_DestroyArr(CanvasLayerArr_T* arr);
void              Canvas_ResizeArr(CanvasLayerArr_T* arr, int32_t newCapacity); // if newCapacity is less than arr's capacity, all the extra layers will be destroyed with "Canvas_DestroyLayer" and memory with newCapacity size will be allocated.
CanvasLayer_T*    Canvas_CreateLayer(SDL_Renderer* ren);
void              Canvas_UpdateLayerTexture(CanvasLayer_T* c); // you need to call this function after you update CanvasLayer_T's "pixel" member
void              Canvas_DestroyLayer(CanvasLayer_T* c);

#ifdef __cplusplus
}
#endif

