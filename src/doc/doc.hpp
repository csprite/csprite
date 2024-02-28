#ifndef CSP_DOC_DOC_HPP_INCLUDED_
#define CSP_DOC_DOC_HPP_INCLUDED_ 1
#pragma once

#include <cstring>

#include "types.h"
#include "pixel/pixel.h"
#include "imbase/texture.hpp"

#include "image/image.hpp"
#include "image/blender.h"

// `Destroy()` resets the value to default, i.e. `nullptr`
// Thus can be used to check if the struct is valid or not.

struct Doc {
	Image image;
	Pixel* render = nullptr;
	ImBase::Texture* renderTex = nullptr;

	bool Create(u32 w, u32 h);

	inline void ClearRender() {
		std::memset(render, 0, image.w * image.h * sizeof(Pixel));
		renderTex->Update((unsigned char*)render, 0, 0, image.w, image.h);
	}

	// Blends The `image` to `render` & updates the `renderTex`
	inline void Render(const mm_RectU32& dirtyArea) {
		BlendImage(image, dirtyArea, render);
		renderTex->Update(
			(unsigned char*)render,
			dirtyArea.min_x, dirtyArea.min_y,
			dirtyArea.max_x - dirtyArea.min_x,
			dirtyArea.max_y - dirtyArea.min_y
		);
	}

	void Destroy();
};

#endif // CSP_DOC_DOC_HPP_INCLUDED_

