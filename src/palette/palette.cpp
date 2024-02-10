#include "types.hpp"
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

#include "fs/fs.hpp"
#include "app/fswrapper.hpp"

namespace Fs = FileSystem;

static std::vector<String> PaletteFiles;

void PaletteHelper::UpdateEntries() {
	PaletteFiles.clear();
	Fs::ListDir(Fs::GetPalettesDir(), [&](const String& entryName, bool isFile) -> bool {
		if (isFile) {
			PaletteFiles.push_back(entryName);
		}
		return true;
	});
}

void PaletteHelper::ListAll(OnListCB cb) {
	for (auto const& filePath : PaletteFiles) {
		cb(filePath.c_str());
	}
}

void PaletteHelper::LoadDefault(Palette& pal) {
	// https://lospec.com/palette-list/sweetie-16
	pal.Add(Pixel{ 0x1A, 0x1C, 0x2C, 0xFF });
	pal.Add(Pixel{ 0x5D, 0x27, 0x5D, 0xFF });
	pal.Add(Pixel{ 0xB1, 0x3E, 0x53, 0xFF });
	pal.Add(Pixel{ 0xEF, 0x7D, 0x57, 0xFF });
	pal.Add(Pixel{ 0xFF, 0xCD, 0x75, 0xFF });
	pal.Add(Pixel{ 0xA7, 0xF0, 0x70, 0xFF });
	pal.Add(Pixel{ 0x38, 0xB7, 0x64, 0xFF });
	pal.Add(Pixel{ 0x25, 0x71, 0x79, 0xFF });
	pal.Add(Pixel{ 0x29, 0x36, 0x6F, 0xFF });
	pal.Add(Pixel{ 0x3B, 0x5D, 0xC9, 0xFF });
	pal.Add(Pixel{ 0x41, 0xA6, 0xF6, 0xFF });
	pal.Add(Pixel{ 0x73, 0xEF, 0xF7, 0xFF });
	pal.Add(Pixel{ 0xF4, 0xF4, 0xF4, 0xFF });
	pal.Add(Pixel{ 0x94, 0xB0, 0xC2, 0xFF });
	pal.Add(Pixel{ 0x56, 0x6C, 0x86, 0xFF });
	pal.Add(Pixel{ 0x33, 0x3C, 0x57, 0xFF });
}

