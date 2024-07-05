#ifndef SRC_DOC_DOC_H_INCLUDED_
#define SRC_DOC_DOC_H_INCLUDED_
#pragma once

#include "image/image.h"
#include "palette/palette.h"

typedef struct {
	char* filePath;
	image_t image;
	palette_t palette;
} document_t;

#endif
