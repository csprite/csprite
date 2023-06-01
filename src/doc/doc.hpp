#ifndef CSP_DOC_DOC_HPP_INCLUDED_
#define CSP_DOC_DOC_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "pixel/pixel.hpp"
#include "renderer/canvas.hpp"

struct DocLayer {
	String name;
	Pixel* pixels = nullptr;

	~DocLayer();
};

struct Doc {
	u16 w = 0, h = 0;
	Vector<DocLayer*> layers;
	Pixel* finalRender = nullptr;

	/* this is stored as pointer because it can be optional and not initialized
	   if rendering without initializing a graphics context or a window */
	Canvas* canvas = nullptr;

	/* headless means no calls to any graphics apis or windows will be done
	   as they are not available. */
	bool headless = false;

	~Doc();

	inline std::size_t GetTotalPixels() { return w * h; }

	bool CreateNew(u16 w, u16 h, bool headless = false);
	void Render(RectI32& dirtyArea);

	void AddLayer(const char* name);
	void RemoveLayer(u16 index); // you need to call Render() yourself after this.
};

#endif // CSP_DOC_DOC_HPP_INCLUDED_