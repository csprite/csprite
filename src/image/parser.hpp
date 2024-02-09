#ifndef CSP_DOC_PARSER_PARSER_HPP_INCLUDED_
#define CSP_DOC_PARSER_PARSER_HPP_INCLUDED_
#pragma once

#include "types.hpp"
#include "image/image.hpp"

namespace ImageParser {
	bool Parse(Image& img, const String filePath);
}

#endif // CSP_DOC_PARSER_PARSER_HPP_INCLUDED_

