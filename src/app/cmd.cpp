#include <cstddef>
#include <cstring>

#include "cmd.hpp"
#include "imgui/imgui.h"
#include "log/log.h"

#include "image/parser.hpp"
#include "image/writer.hpp"

#include "sfd.h"

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
			Cmd::Execute(Cmd::Type::Center_Viewport, state);

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

bool Cmd::Execute(Cmd::Type t, DocumentState& state) {
	switch (t) {
		case New_File: {
			IsOpen_NewDocumentWin = true;
			break;
		}
		case Open_File: {
			sfd_Options opt = {};
			opt.save = 0;
			opt.title = "Open File";
			opt.filter_name = "Images";
			opt.filter = IMAGE_PARSER_SUPPORTED_EXTENSIONS;
			const char* filePath = sfd_open_dialog(&opt);

			if (filePath != NULL) {
				Image img;
				if (ImageParser::Parse(img, filePath)) {
					state.doc.Destroy();
					state.doc.Create(img.w, img.h);
					state.doc.image = std::move(img);
					state.doc.Render({ 0, 0, state.doc.image.w, state.doc.image.h });
					Cmd::Execute(Cmd::Type::Center_Viewport, state);
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
			if (state.filePath.empty()) {
				return Cmd::Execute(Cmd::Type::SaveAs_File, state);
			} else {
				return ImageWriter::Write(state.doc.image, state.filePath);
			}
			break;
		}
		case SaveAs_File: {
			sfd_Options opt = {};
			opt.save = 1;
			opt.title = "Save File";
			opt.filter_name = "Images";
			opt.filter = IMAGE_WRITER_SUPPORTED_EXTENSIONS;

			const char* filePath = sfd_open_dialog(&opt);

			if (filePath != NULL) {
				if (ImageWriter::Write(state.doc.image, filePath)) {
					state.filePath = filePath;
				}
			}
			break;
		}
		case Center_Viewport: {
			state.tManager.UpdateViewportScale(state.doc);

			state.tManager.viewport.x = (ImGui::GetIO().DisplaySize.x / 2) - (state.tManager.viewportScale / 2);
			state.tManager.viewport.y = (ImGui::GetIO().DisplaySize.y / 2) - (state.tManager.viewportScale / 2);
			break;
		}
		case ZoomIn_Viewport: {
			if (state.tManager.viewportScale < 1000) {
				state.tManager.viewportScale += 0.15;
				state.tManager.UpdateViewportScale(state.doc);
			}
			break;
		}
		case ZoomOut_Viewport: {
			if (state.tManager.viewportScale >= 0.30) {
				state.tManager.viewportScale -= 0.15;
				state.tManager.UpdateViewportScale(state.doc);
			}
			break;
		}
	}

	return true;
}
