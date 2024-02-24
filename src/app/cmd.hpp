#ifndef CSP_APP_CMD_HPP_INCLUDED_
#define CSP_APP_CMD_HPP_INCLUDED_ 1
#pragma once

namespace Cmd {
	enum Type {
		New_File,        // (Doc* doc, u32 widthNew, u32 heightNew)
		Open_File,       // (Doc* doc, Tool::Manager* mgr)
		Save_File,       // (const Image* img, String* filePath)
		SaveAs_File,     // (const Image* img, String* filePath)
		Center_Viewport, // (Tool::Manager* mgr, const Doc* doc)
		ZoomIn_Viewport, // (Tool::Manager* mgr, const Doc* doc)
		ZoomOut_Viewport // (Tool::Manager* mgr, const Doc* doc)
	};

	bool Execute(Cmd::Type t, ...);
}

#endif // CSP_APP_CMD_HPP_INCLUDED_
