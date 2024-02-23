#ifndef CSP_IMAGE_IMAGE_HPP_INCLUDED_
#define CSP_IMAGE_IMAGE_HPP_INCLUDED_ 1
#pragma once

#include "types.h"
#include "pixel/pixel.h"

// `Destroy()` resets the value to default, i.e. `nullptr` or `0`
// Thus can be used to check if the structs below are valid.

// https://lodev.org/cgtutor/imagearithmetic.html
enum Blend {
	Normal,
	Addition,   // src + dst
	Subtraction,// src - dst
	Difference, // abs(src - dst)
	Multiply,   // ((src/255) * (dst/255)) * 255
	Screen,     // (dst + src - (((src/255) * (dst/255)) * 255))
	Darken,     // min(dst, src)
	Lighten,    // max(dst, src)

	Count // For Development
};

inline const char* BlendModeToString(Blend mode) {
	switch (mode) {
		case Normal:      return "Normal";
		case Addition:    return "Addition";
		case Subtraction: return "Subtraction";
		case Difference:  return "Difference";
		case Multiply:    return "Multiply";
		case Screen:      return "Screen";
		case Darken:      return "Darken";
		case Lighten:     return "Lighten";
		case Count:       return "Unknown";
	}
	return "Unknown";
}

struct Layer {
	String name = "New Layer";
	Blend  blend = Blend::Normal;
	u8     opacity = 255;
	Pixel* pixels = nullptr;

	bool Create(u32 w, u32 h);
	void Destroy();
};

struct Image {
	u32 w = 0, h = 0;
	Vector<Layer> Layers;

	bool Create(u32 w, u32 h);

	void AddLayer();
	void RemoveLayer(u32 index); // NOTE: doesn't check if the index is valid/in-range or not

	void Destroy();
};

#endif // CSP_IMAGE_IMAGE_HPP_INCLUDED_
