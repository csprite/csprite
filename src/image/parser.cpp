#include "image/parser.hpp"

#include "stb_image.h"
#include "log/log.h"

bool ImageParser::Parse(Image& img, const String filePath) {
	i32 width = 0, height = 0, c = 0;
	u8* image = stbi_load(filePath.c_str(), &width, &height, &c, 4);

	if (image == NULL || width < 1 || height < 1) {
		log_error("stbi_load(...) - %s", stbi_failure_reason());
		return false;
	}

	img.Create(width, height);
	img.AddLayer();
	Layer& layer = img.Layers[0];

	for (i32 y = 0; y < height; ++y) {
		for (i32 x = 0; x < width; ++x) {
			Pixel& p = layer.pixels[(y * width) + x];
			u8* pSrc = &image[((y * width) + x) * 4];
			p.r = *(pSrc + 0);
			p.g = *(pSrc + 1);
			p.b = *(pSrc + 2);
			p.a = *(pSrc + 3);
		}
	}

	stbi_image_free(image);
	image = nullptr;

	return true;
}

