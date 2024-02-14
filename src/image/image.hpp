#ifndef CSP_IMAGE_IMAGE_HPP_INCLUDED_
#define CSP_IMAGE_IMAGE_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "pixel/pixel.hpp"

// `Destroy()` resets the value to default, i.e. `nullptr` or `0`
// Thus can be used to check if the structs below are valid.

// https://lodev.org/cgtutor/imagearithmetic.html
enum Blend {
	Alpha,
	Addition,   // src + dst
	Subtraction,// src - dst
	Difference, // abs(src - dst)
	Multiply,   // ((src/255) * (dst/255)) * 255
	Screen,     // (dst + src - (((src/255) * (dst/255)) * 255))
};

struct Layer {
	String name;
	Blend  blend = Blend::Alpha;
	Pixel* pixels = nullptr;

	bool Create(u32 w, u32 h, String name);
	void Destroy();
};

struct Image {
	u32 w = 0, h = 0;
	Vector<Layer> Layers;

	bool Create(u32 w, u32 h);

	void AddLayer(String name);
	void RemoveLayer(u32 index); // NOTE: doesn't check if the index is valid/in-range or not

	void Destroy();
};

#endif // CSP_IMAGE_IMAGE_HPP_INCLUDED_

