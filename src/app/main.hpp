#ifndef CSP_MAIN_HPP_INCLUDED_
#define CSP_MAIN_HPP_INCLUDED_
#pragma once

#include "types.h"
#include "doc/doc.hpp"
#include "palette/palette.hpp"
#include "tools/ToolManager.hpp"

struct Editor {
	Doc           doc;
	Tool::Manager mgr;
	Palette       pal;
	String   filePath;
};

#endif // CSP_MAIN_HPP_INCLUDED_
