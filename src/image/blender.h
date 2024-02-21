#ifndef CSP_IMAGE_BLENDER_H_INCLUDED_
#define CSP_IMAGE_BLENDER_H_INCLUDED_

#include "types.h"
#include "image/image.hpp"

Pixel BlendPixel(Blend mode, Pixel src, Pixel backdrop);
Pixel BlendAlpha(Pixel src, Pixel backdrop, u8 opacity);

// NOTE: outBuff is expected to be same size as `img.w` & `img.h`
void BlendRect(const Image& img, const mm_RectU32& dirtyArea, Pixel* outBuff, bool checkerboard = true);

#endif // CSP_IMAGE_BLENDER_H_INCLUDED_
