#ifndef CSP_IMAGE_BLENDER_HPP_INCLUDED_
#define CSP_IMAGE_BLENDER_HPP_INCLUDED_

#include "types.hpp"
#include "image/image.hpp"

namespace Blender {
	// Dirty Area Rect Containing Top Left & Bottom Right coords
	// NOTE: outBuff is expected to be same size as `img.w` & `img.h`
	void Blend(const Image& img, const RectU32& dirtyArea, Pixel* outBuff, bool checkerboard = true);
};

#endif // CSP_IMAGE_BLENDER_HPP_INCLUDED_

