#include "palette/palette.hpp"

Pixel& Palette::operator[](u32 i) {
	return Colors[i];
};

void Palette::Add(Pixel color) {
	if (!this->Contains(color)) {
		Colors.push_back(color);
	}
}

bool Palette::Contains(Pixel& color) const {
	for (const Pixel& pCol : Colors) {
		if (pCol == color) return true;
	}
	return false;
}

void Palette::Remove(Pixel& color) {
	for (i32 i = 0; i < (i32)Colors.size(); ++i) {
		if (color == Colors[i]) {
			Colors.erase(Colors.begin() + i);
		}
	}
}
