#ifndef CSP_SRC_PIXEL_PIXEL_H_INCLUDED_
#define CSP_SRC_PIXEL_PIXEL_H_INCLUDED_ 1
#pragma once

#include "types.h"

#ifdef __cplusplus
	#include "imgui/imgui.h"
#endif

struct Pixel {
	u8 r = 0, g = 0, b = 0, a = 0;
#ifdef __cplusplus
	inline operator ImVec4() {
		return { r / 255.f, g / 255.f, b / 255.f, a / 255.f };
	}

	inline bool operator == (const Pixel& rhs) const {
		return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
	}

	inline bool operator != (const Pixel& rhs) const {
		return !(*this == rhs);
	}

	inline explicit operator u8*() {
		return &r;
	}
#endif
};

#endif // CSP_SRC_PIXEL_PIXEL_H_INCLUDED_
