#ifndef IMBASE_TEXTURE_HPP_INCLUDED_
#define IMBASE_TEXTURE_HPP_INCLUDED_ 1
#pragma once

namespace ImBase {
	// Note: Pixels are expected to be aligned in R-G-B-A order in memory
	struct Texture {
		unsigned int id = 0; // when using with ImGui::Image, use reinterpret_cast<ImTextureID>()

		Texture(int width, int height, unsigned char* pixels);
		void Update(unsigned char* pixels);
		~Texture();

	private:
		int width = 0, height = 0;
	};
}

#endif // IMBASE_TEXTURE_HPP_INCLUDED_

