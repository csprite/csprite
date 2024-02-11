#include <algorithm>

#include "palette/parser.hpp"
#include "image/image.hpp"
#include "image/parser.hpp"

bool PaletteParser::Parse(Palette& pal, const String filePath) {
	Image palImg;
	if (!ImageParser::Parse(palImg, filePath)) {
		return false;
	}

	for (size_t y = 0; y < palImg.h; y++) {
		for (size_t x = 0; x < palImg.w; x++) {
			for (size_t i = 0; i < palImg.Layers.size(); i++) {
				Pixel p = palImg.Layers[i].pixels[(y * palImg.w) + x];
				if (std::find(pal.Colors.begin(), pal.Colors.end(), p) == pal.Colors.end()) {
					pal.Colors.push_back(p);
				}
			}
		}
	}

	palImg.Destroy();

	return true;
}

