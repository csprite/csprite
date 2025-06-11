#include "bitmap/bitmap.h"
#include "base/types.h"
#include "stb_image.h"
#include <string.h>

B32 bitmap_is_zero(Bitmap b) {
	Bitmap zero = {0};
	return memcmp(&b, &zero, sizeof(b)) == 0;
}

Bitmap bitmap_from_null(Arena* a, U64 width, U64 height) {
	Bitmap b = {
		.width = width,
		.height = height,
		.pixels = arena_alloc_zero(a, width * height * sizeof(Pixel))
	};
	return b;
}

Bitmap bitmap_from_filepath(Arena* a, String8 filePath) {
	Bitmap b = {0};

	S32 w = 0, h = 0, c = 0;
	stbi_uc* data = stbi_load((const char*)filePath.str, &w, &h, &c, 4);
	if (data == NULL) {
		printf("[LOG] stbi_load(...) returned NULL\n");
		return b;
	} else if (w < 1 || h < 1 || c < 1 || c > 4) {
		printf("[LOG] Invalid/Unexpected Image Properties: %dx%dx%d\n", w, h, c);
		stbi_image_free(data);
		return b;
	}

	b.width = w;
	b.height = h;
	b.pixels = arena_alloc(a, w * h * sizeof(Pixel));

	for EachIndex(i, (U64)(w * h)) {
		Pixel p = { .r = 0, .g = 0, .b = 0, .a = 255 };
		switch (c) {
			case 1: {
				p.r = p.g = p.b = data[i * 4];
				break;
			}
			case 2: {
				p.r = data[(i * 4) + 0];
				p.g = data[(i * 4) + 1];
				break;
			}
			case 3: {
				p.r = data[(i * 4) + 0];
				p.g = data[(i * 4) + 1];
				p.b = data[(i * 4) + 2];
				break;
			}
			case 4: {
				p.r = data[(i * 4) + 0];
				p.g = data[(i * 4) + 1];
				p.b = data[(i * 4) + 2];
				p.a = data[(i * 4) + 3];
				break;
			}
		}
		b.pixels[i] = p;
	}

	stbi_image_free(data);

	return b;
}
