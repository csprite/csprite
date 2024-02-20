#ifndef CSP_APP_CMD_HPP_INCLUDED_
#define CSP_APP_CMD_HPP_INCLUDED_ 1
#pragma once

#include "app/i18n/strings.hpp"
#include "doc/doc.hpp"
#include "tools/ToolManager.hpp"
#include "app/main.hpp"

namespace Cmd {
	enum Type {
		Open_File,
		New_File,
		Save_File,
		SaveAs_File,
		Center_Viewport,
		ZoomIn_Viewport,
		ZoomOut_Viewport
	};

	void Draw(const UISTR_Arr& Lang, DocumentState& state);
	bool Execute(Cmd::Type t, DocumentState& state);
}

#endif // CSP_APP_CMD_HPP_INCLUDED_
