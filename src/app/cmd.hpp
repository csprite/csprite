#ifndef CSP_APP_CMD_HPP_INCLUDED_
#define CSP_APP_CMD_HPP_INCLUDED_ 1
#pragma once

#include "app/i18n/strings.hpp"
#include "doc/doc.hpp"
#include "tools/ToolManager.hpp"
#include "app/main.hpp"

namespace Cmd {
	enum Type {
		New_File,        // ()
		Open_File,       // (Doc* doc, Tool::Manager* mgr)
		Save_File,       // (const Image* img, String* filePath)
		SaveAs_File,     // (const Image* img, String* filePath)
		Center_Viewport, // (Tool::Manager* mgr, const Doc* doc)
		ZoomIn_Viewport, // (Tool::Manager* mgr, const Doc* doc)
		ZoomOut_Viewport // (Tool::Manager* mgr, const Doc* doc)
	};

	void Draw(const UISTR_Arr& Lang, DocumentState& state);
	bool Execute(Cmd::Type t, ...);
}

#endif // CSP_APP_CMD_HPP_INCLUDED_
