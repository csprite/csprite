#ifndef IMBASE_WINDOW_HPP_INCLUDED_
#define IMBASE_WINDOW_HPP_INCLUDED_ 1
#pragma once

namespace ImBase {
	namespace Window {
		int  Init(int width, int height, const char* const title);
		void SetIcon(int width, int height, unsigned char* pixels);
		void SetTitle(const char* const title);
		void SetBG(unsigned char r, unsigned char g, unsigned char b);
		bool ShouldClose();
		void NewFrame();
		void EndFrame();
		void Destroy();
	}
}

#endif // IMBASE_WINDOW_HPP_INCLUDED_

