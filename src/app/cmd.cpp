#include <cstddef>
#include <cstring>
#include <cstdarg>

#include "cmd.hpp"
#include "imgui/imgui.h"
#include "log/log.h"

#include "image/parser.hpp"
#include "image/writer.hpp"

#include "sfd.h"
#include "tools/ToolManager.hpp"

static bool
	IsOpen_NewDocumentWin = false;

static int NewCanvasRes[2] = {60, 40};

void Cmd::Draw(const UISTR_Arr& Lang, DocumentState& state) {
	#define BEGIN_POPUP(name, flags) if (IsOpen_NewDocumentWin && ImGui::Begin(name, NULL, flags)) {
	#define END_POPUP() ImGui::End(); }

	BEGIN_POPUP(Lang[UISTR::Popup_NewDocument], ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Modal)
		ImGui::InputInt(Lang[UISTR::Popup_NewDocument_WidthInput], &NewCanvasRes[0], 1, 1, 0);
		ImGui::InputInt(Lang[UISTR::Popup_NewDocument_HeightInput], &NewCanvasRes[1], 1, 1, 0);

		if (ImGui::Button(Lang[UISTR::Popup_NewDocument_OkButton])) {
			state.doc.Destroy();
			state.doc.Create(NewCanvasRes[0], NewCanvasRes[1]);
			state.doc.image.AddLayer("New Layer");
			state.doc.Render({ 0, 0, state.doc.image.w, state.doc.image.h });
			Cmd::Execute(Cmd::Type::Center_Viewport, state.tManager, &state.doc);

			IsOpen_NewDocumentWin = false;
		}
		ImGui::SameLine();
		if (ImGui::Button(Lang[UISTR::Popup_NewDocument_CancelButton])) {
			IsOpen_NewDocumentWin = false;
		}
	END_POPUP()

	#undef END_POPUP
	#undef BEGIN_POPUP
}

bool Cmd::Execute(Cmd::Type t, ...) {
	#define BEGIN_ARGS(num) va_list args; va_start(args, num)
	#define GET_ARG(type, name) type name = va_arg(args, type)
	#define END_ARGS() va_end(args)

	switch (t) {
		case New_File: {
			IsOpen_NewDocumentWin = true;
			break;
		}
		case Open_File: {
			BEGIN_ARGS(2);
				GET_ARG(Doc*, doc);
				GET_ARG(Tool::Manager*, mgr);
			END_ARGS();

			sfd_Options opt = {};
			opt.save = 0;
			opt.title = "Open File";
			opt.filter_name = "Images";
			opt.filter = IMAGE_PARSER_SUPPORTED_EXTENSIONS;
			const char* filePath = sfd_open_dialog(&opt);

			if (filePath != NULL) {
				Image img;
				if (ImageParser::Parse(img, filePath)) {
					doc->Destroy();
					doc->Create(img.w, img.h);
					doc->image = std::move(img);
					doc->Render({ 0, 0, doc->image.w, doc->image.h });
					Cmd::Execute(Cmd::Type::Center_Viewport, mgr, doc);
				}
			} else {
				const char* ErrorLast = sfd_get_error();
				if (ErrorLast != NULL) {
					log_error("Error: %s (%s)", ErrorLast, strerror(errno));
				}
			}
			break;
		}
		case Save_File: {
			BEGIN_ARGS(2);
				GET_ARG(const Image*, img);
				GET_ARG(String*, fp);
			END_ARGS();

			if (fp->empty()) {
				return Cmd::Execute(Cmd::Type::SaveAs_File, img, fp);
			} else {
				return ImageWriter::Write(*img, *fp);
			}
			break;
		}
		case SaveAs_File: {
			BEGIN_ARGS(2);
				GET_ARG(const Image*, img);
				GET_ARG(String*, fp);
			END_ARGS();

			sfd_Options opt = {};
			opt.save = 1;
			opt.title = "Save File";
			opt.filter_name = "Images";
			opt.filter = IMAGE_WRITER_SUPPORTED_EXTENSIONS;

			const char* filePath = sfd_open_dialog(&opt);

			if (filePath != NULL) {
				if (ImageWriter::Write(*img, filePath)) {
					*fp = filePath;
				}
			}
			break;
		}
		case Center_Viewport: {
			BEGIN_ARGS(2);
				GET_ARG(Tool::Manager*, mgr);
				GET_ARG(const Doc*, doc);
			END_ARGS();

			mgr->UpdateViewportScale(*doc);

			mgr->viewport.x = (ImGui::GetIO().DisplaySize.x / 2) - (mgr->viewportScale / 2);
			mgr->viewport.y = (ImGui::GetIO().DisplaySize.y / 2) - (mgr->viewportScale / 2);
			break;
		}
		case ZoomIn_Viewport: {
			BEGIN_ARGS(2);
				GET_ARG(Tool::Manager*, mgr);
				GET_ARG(const Doc*, doc);
			END_ARGS();

			if (mgr->viewportScale < 1000) {
				mgr->viewportScale += 0.15;
				mgr->UpdateViewportScale(*doc);
			}
			break;
		}
		case ZoomOut_Viewport: {
			BEGIN_ARGS(2);
				GET_ARG(Tool::Manager*, mgr);
				GET_ARG(const Doc*, doc);
			END_ARGS();

			if (mgr->viewportScale >= 0.30) {
				mgr->viewportScale -= 0.15;
				mgr->UpdateViewportScale(*doc);
			}
			break;
		}
	}

	return true;
}
