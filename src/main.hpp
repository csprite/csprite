#ifndef CSP_MAIN_HPP_INCLUDED_
#define CSP_MAIN_HPP_INCLUDED_
#pragma once

#include "types.hpp"
#include "doc/doc.hpp"

struct DocumentState {
	u16 SelectedLayer;
	u16 CanvasZoom;
	Pixel SelectedColor;

	Doc doc;
};

// Canvas Zoom
void ZoomNCenterVP(); // Zooms And Centers The Viewport.
void AdjustZoom(bool increase);

#endif // CSP_MAIN_HPP_INCLUDED_
