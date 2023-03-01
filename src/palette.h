#ifndef PALETTE_H
#define PALETTE_H

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

typedef struct {
	uint8_t r, g, b, a;
} Color_T;

struct Palette {
	std::string name;
	std::string author;
	std::vector<Color_T> colors;

	void AddColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	void RemoveColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255); // removes the duplicate colors too if found.
};

struct PaletteManager {
	std::vector<Palette>* presets;
	Palette palette;
	int32_t SelectedColorIdx;
	Color_T PrimaryColor;

	PaletteManager();
	~PaletteManager();

	void SetPreset(Palette& p);
	void SetSelectedColorIdx(int32_t idx);
};

std::vector<Palette>* Palette_LoadAll();
void Palette_ReleaseAll(std::vector<Palette>* palettes);

Palette* Palette_LoadCsv(const char* csvText); // Load lospec format .csv palettes
char* Palettes_GetDir(); // directory where csprite loads the palettes from

#endif // PALETTE_H


