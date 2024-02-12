#ifndef CSP_MAIN_HPP_INCLUDED_
#define CSP_MAIN_HPP_INCLUDED_
#pragma once

#include "types.hpp"
#include "doc/doc.hpp"
#include "palette/palette.hpp"
#include "tools/ToolManager.hpp"

struct DocumentState {
	Doc doc;
	Tool::Manager tManager;
	Palette palette;
	String filePath;
	u32 PaletteIndex = 0;
};

inline void ZoomNCenterVP(Tool::Manager& mgr, const Doc& doc);
inline void AdjustZoom(bool Increase, Tool::Manager& mgr, const Doc& doc);

#endif // CSP_MAIN_HPP_INCLUDED_
