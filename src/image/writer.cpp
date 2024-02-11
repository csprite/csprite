#include <cstring>

#include "types.hpp"
#include "stb_image_write.h"
#include "image/writer.hpp"
#include "image/blender.hpp"
#include "log/log.h"

bool ImageWriter::Write(const Image& img, const String filePath) {
	if (img.w < 1 || img.h < 1) return false;

	Pixel* finalRender = new Pixel[img.w * img.h]{ 0, 0, 0, 0 };
	Blender::Blend(img, { 0, 0, img.w, img.h }, finalRender, false);

	const char* path = filePath.c_str();
	size_t pathLen = filePath.size();

	if (std::strncmp(".png", &path[pathLen - 4], 4) == 0) {
		stbi_write_png_compression_level = 9;
		stbi_write_png(path, img.w, img.h, 4, finalRender, img.w * sizeof(Pixel));
	} else if (std::strncmp(".jpg", &path[pathLen - 4], 4) == 0 || std::strncmp(".jpeg", &path[pathLen - 5], 5) == 0) {
		stbi_write_jpg(path, img.w, img.h, 4, finalRender, 100);
	} else if (std::strncmp(".bmp", &path[pathLen - 4], 4) == 0) {
		stbi_write_bmp(path, img.w, img.h, 4, finalRender);
	} else if (std::strncmp(".tga", &path[pathLen - 4], 4) == 0) {
		stbi_write_tga(path, img.w, img.h, 4, finalRender);
	} else {
		delete[] finalRender;
		log_error("Unknown file extension: '%s'", filePath.c_str());
		return false;
	}

	delete[] finalRender;
	return true;
}

