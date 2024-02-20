#ifndef CSP_MAIN_HPP_INCLUDED_
#define CSP_MAIN_HPP_INCLUDED_
#pragma once

#include "types.hpp"
#include "doc/doc.hpp"
#include "palette/palette.hpp"
#include "tools/ToolManager.hpp"

#include <limits>
// Clamps "a" to the min & max value of "T", without overflowing.
#define MIN_MAX_OF_TYPE(a, T)               \
	static_cast<T>(                         \
		MIN_MAX(                            \
			a,                              \
			std::numeric_limits<T>().min(), \
			std::numeric_limits<T>().max()  \
		)                                   \
	)

struct DocumentState {
	Doc doc;
	Tool::Manager tManager;
	Palette palette;
	String filePath;
	u32 PaletteIndex = 0;
};

#endif // CSP_MAIN_HPP_INCLUDED_
