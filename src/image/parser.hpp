#ifndef CSP_DOC_PARSER_PARSER_HPP_INCLUDED_
#define CSP_DOC_PARSER_PARSER_HPP_INCLUDED_
#pragma once

#include "types.h"
#include "image/image.hpp"

// Todo: Automate This Process in Future
#define IMAGE_PARSER_SUPPORTED_EXTENSIONS "*.png|*.jpg|*.jpeg|*.bmp|*.psd|*.tga"

namespace ImageParser {
	bool Parse(Image& img, const String filePath);
}

#endif // CSP_DOC_PARSER_PARSER_HPP_INCLUDED_

