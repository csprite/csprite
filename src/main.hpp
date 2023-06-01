#ifndef CSP_MAIN_HPP_INCLUDED_
#define CSP_MAIN_HPP_INCLUDED_
#pragma once

#include "types.hpp"
#include "doc/doc.hpp"
#include "palette/palette.hpp"

struct DocumentState {
	Pixel SelectedColor;
	u16 PaletteIndex;
	Palette palette;

	u16 CanvasZoom;
	String CanvasZoomText;

	u16 SelectedLayer;
	Doc doc;
};

// Canvas Zoom
void ZoomNCenterVP(); // Zooms And Centers The Viewport.
void AdjustZoom(bool increase);

#endif // CSP_MAIN_HPP_INCLUDED_
