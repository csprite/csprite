#include <iostream>

#include "types.hpp"
#include "doc/parser/parser.hpp"
#include "stb_image.h"

Doc* Parser::ParseImageFile(const char *const filePath) {
	i32 width = 0, height = 0, c = 0;
	u8* image = stbi_load(filePath, &width, &height, &c, 4);

	if (image == NULL || width < 1 || height < 1 || c != 4) {
		std::cout << "Error: stbi_load(...)" << std::endl;
		return nullptr;
	}

	Doc* d = new Doc();
	d->CreateNew(width, height);
	d->AddLayer("New Layer");
	DocLayer* dL = d->layers[0];

	for (i32 y = 0; y < height; ++y) {
		for (i32 x = 0; x < width; ++x) {
			Pixel& p = dL->pixels[(y * width) + x];
			u8* pSrc = &image[((y * width) + x) * 4];
			p.r = *(pSrc + 0);
			p.g = *(pSrc + 1);
			p.b = *(pSrc + 2);
			p.a = *(pSrc + 3);
		}
	}

	stbi_image_free(image);
	image = nullptr;

	return d;
}

