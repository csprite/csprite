#ifndef CSP_IMAGE_WRITER_HPP_INCLUDED_
#define CSP_IMAGE_WRITER_HPP_INCLUDED_ 1
#pragma once

#include "types.h"
#include "image/image.hpp"

// Todo: Automate This Process in Future
#define IMAGE_WRITER_SUPPORTED_EXTENSIONS "*.png|*.jpg|*.jpeg|*.bmp|*.psd|*.tga"

namespace ImageWriter {
	bool Write(const Image& img, const String filePath);
}

#endif // CSP_IMAGE_WRITER_HPP_INCLUDED_

