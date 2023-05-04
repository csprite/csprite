#ifndef CSP_CANVAS_CANVAS_HPP_INCLUDED_
#define CSP_CANVAS_CANVAS_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "doc/bitmap.hpp"

// Functions For Managing Bitmap Renderer

namespace Canvas {
	void Blend(Bitmap::Bitmap& bitmap, Rect& dirtyArea);
}

#endif
