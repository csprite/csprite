#ifndef CSP_PALETTE_PALETTE_HPP_INCLUDED_
#define CSP_PALETTE_PALETTE_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "pixel/pixel.hpp"

struct Palette {
	Vector<Pixel> Colors;

	Pixel& operator[](u32 i);

	void Add(Pixel color); // only adds if the color isn't in the palette
	bool Contains(Pixel& color) const; // returns true if the color is in the palette
	void Remove(Pixel& color); // removes the color in the palette (duplicates are removed too)
};

#endif // CSP_PALETTE_PALETTE_HPP_INCLUDED_
