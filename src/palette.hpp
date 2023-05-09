#ifndef CSP_PALETTE_HPP_INCLUDED_
#define CSP_PALETTE_HPP_INCLUDED_ 1
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

#include "pixel/pixel.hpp"

struct Palette {
	std::string name;
	std::string author;
	std::vector<Pixel> colors;

	void AddColor(Pixel color);
	void AddColor(Pixel& color);
	void RemoveColor(Pixel& color); // removes the duplicate colors too if found.
};

struct PaletteManager {
	std::vector<Palette>* presets;
	Palette palette;
	int32_t SelectedColorIdx;
	Pixel   PrimaryColor;

	PaletteManager();
	~PaletteManager();

	void SetPreset(Palette& p);
	void SetSelectedColorIdx(int32_t idx);
};

std::vector<Palette>* Palette_LoadAll();
void Palette_ReleaseAll(std::vector<Palette>* palettes);

Palette* Palette_LoadCsv(const char* csvText); // Load lospec format .csv palettes
char* Palettes_GetDir(); // directory where csprite loads the palettes from

#endif // CSP_PALETTE_HPP_INCLUDED_
