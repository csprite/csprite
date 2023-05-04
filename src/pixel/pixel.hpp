#ifndef CSP_DOC_PIXEL_HPP_INCLUDED_
#define CSP_DOC_PIXEL_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"

struct Pixel {
#if TARGET_IS_BIGENDIAN == 0
	u8 r, g, b, a;
#else
	u8 a, b, g, r;
#endif

	Pixel& operator = (Pixel& rhs);
	inline bool operator == (const Pixel& rhs) const;
	inline bool operator != (const Pixel& rhs) const;
};

#endif // CSP_DOC_PIXEL_HPP_INCLUDED_
