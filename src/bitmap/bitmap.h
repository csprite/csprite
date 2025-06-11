#ifndef SRC_BITMAP_BITMAP_H
#define SRC_BITMAP_BITMAP_H 1

#include "base/types.h"
#include "base/arena.h"
#include "base/string.h"

typedef struct {
	U64 width, height;
	Pixel* pixels;
} Bitmap;

B32 bitmap_is_zero(Bitmap b);
Bitmap bitmap_from_null(Arena* a, U64 width, U64 height);
Bitmap bitmap_from_filepath(Arena* a, String8 filePath);

#endif // SRC_BITMAP_BITMAP_H
