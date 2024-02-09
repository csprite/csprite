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

