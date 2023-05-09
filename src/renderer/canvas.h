#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <SDL.h>
#include "../history.h"
#include "types.hpp"
#include "pixel/pixel.hpp"

struct CanvasLayer {
	String       name;
	SDL_Texture* tex;
	Pixel*       pixels;
	History_T*   history;

	CanvasLayer(SDL_Renderer* ren, int32_t w, int32_t h, std::string name = "New Layer");
	~CanvasLayer();
};

class CanvasLayer_Manager {
public:
	std::vector<CanvasLayer*> layers;
	CanvasLayer*  layer;
	SDL_Texture*  render;
	SDL_Texture*  pattern;
	SDL_Renderer* ren;
	int32_t       dims[2];
	int32_t CurrentLayerIdx;

	/* ren is an instance of SDL_Renderer that should exist for the lime time of the class.
	   w, h is the width & height of the canvas. */
	CanvasLayer_Manager(SDL_Renderer* ren, int32_t w, int32_t h, uint8_t pCol1[3], uint8_t pCol2[3]);
	~CanvasLayer_Manager();

	void AddLayer(std::string name = "New Layer");
	void RemoveLayer(int32_t idx);
	void SetCurrentLayerIdx(int32_t idx);
	void Draw(SDL_Rect* r, int32_t layerToUpdateIdx);
	void ReUploadTexture(int32_t idx);
};

