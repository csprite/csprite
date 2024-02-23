#ifndef CSP_SRC_PALETTE_UI_HPP_INCLUDED_
#define CSP_SRC_PALETTE_UI_HPP_INCLUDED_ 1
#pragma once

#include "palette/palette.hpp"

void Palette_UI_Draw(
	Palette& palette,         // Users Can Select Colors From This or The Color Picker
	u32& colorSelectedIndex,  // If user selects color in palette, this value will be updated
	Pixel& displayColor       // Selected color is stored here, this is useful for storing colors that might not be in the palette because of colors that user can select from the picker
);

#endif // CSP_SRC_PALETTE_UI_HPP_INCLUDED_
