#ifndef CSP_MAIN_HPP_INCLUDED_
#define CSP_MAIN_HPP_INCLUDED_
#pragma once

#include "types.hpp"
#include "doc/doc.hpp"
#include "palette/palette.hpp"

struct DocumentState {
	Pixel SelectedColor = { 255, 255, 255, 255 };
	u16 PaletteIndex = 0;
	Palette palette;

	u16 CanvasZoom = 1;
	u16 SelectedLayer = 0;

	Doc* doc = nullptr;
	String FilePath = "";
};

void ZoomNCenterVP(u32 ZoomLevel, Doc& d);
void AdjustZoom(bool Increase, u32& ZoomLevel, String& ZoomText, Doc& d);

#endif // CSP_MAIN_HPP_INCLUDED_
