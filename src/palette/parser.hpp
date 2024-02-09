#ifndef CSP_PALETTE_PARSER_HPP_INCLUDED_
#define CSP_PALETTE_PARSER_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"
#include "palette/palette.hpp"

namespace PaletteParser {
	bool Parse(Palette& pal, const String filePath);
}

#endif // CSP_PALETTE_PARSER_HPP_INCLUDED_

