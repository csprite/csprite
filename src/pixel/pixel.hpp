#ifndef CSP_DOC_PIXEL_HPP_INCLUDED_
#define CSP_DOC_PIXEL_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"

struct Pixel {
	u8 r, g, b, a;

	Pixel& operator = (Pixel& rhs);
	inline bool operator == (const Pixel& rhs) const;
	inline bool operator != (const Pixel& rhs) const;
};

#endif // CSP_DOC_PIXEL_HPP_INCLUDED_
