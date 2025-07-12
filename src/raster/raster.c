#include "raster/raster.h"
#include "stb_image.h"

Raster rs_init(Arena* a, Rect dim) {
	Raster r = {
		.dim = dim,
		.data = arena_alloc_zero(a, dim.w * dim.h * sizeof(*r.data))
	};
	return r;
}

Raster rs_init_from_file(Arena* a, String8 filepath) {
	Raster r = {0};

	S32 w = 0, h = 0, c = 0;
	stbi_uc* data = stbi_load((const char*)filepath.str, &w, &h, &c, 4);
	if (data == NULL) {
		printf("[LOG] stbi_load(...) returned NULL\n");
		return r;
	} else if (w < 1 || h < 1 || c < 1 || c > 4) {
		printf("[LOG] Invalid/Unexpected Image Properties: %dx%dx%d\n", w, h, c);
		stbi_image_free(data);
		return r;
	}

	r = rs_init(a, rect(w, h));

	#pragma omp parallel for
	for EachIndex(i, r.dim.w * r.dim.h) {
		RGBAU8 p = { .r = 0, .g = 0, .b = 0, .a = 255 };
		switch (c) {
			case 1: {
				p.r = p.g = p.b = data[i * c];
				break;
			}
			case 2: {
				p.r = data[(i * c) + 0];
				p.g = data[(i * c) + 1];
				break;
			}
			case 3: {
				p.r = data[(i * c) + 0];
				p.g = data[(i * c) + 1];
				p.b = data[(i * c) + 2];
				break;
			}
			case 4: {
				p.r = data[(i * c) + 0];
				p.g = data[(i * c) + 1];
				p.b = data[(i * c) + 2];
				p.a = data[(i * c) + 3];
				break;
			}
		}
		r.data[i] = p;
	}

	stbi_image_free(data);

	return r;
}
