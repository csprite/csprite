#include "logger.h"
#include "log/log.h"

Logger Logger::s_Instance;

Logger& Logger::Log() {
	return s_Instance;
}

Logger& GetLogger() {
	return Logger::Log();
}

Logger::Logger() {
	AutoScroll = true;
	Clear();
}

Logger::~Logger() {}

void Logger::Clear() {
	Buf.clear();
	LineOffsets.clear();
	LineOffsets.push_back(0);
}

// For C-API Of The Logger
void Logger::AddLog(int level, const char* file, int line, const char* fmt, va_list args) {
	int old_size = Buf.size();

	Buf.appendf("%-5s %s:%d: ", log_level_string(level), file, line);
	Buf.appendfv(fmt, args);
	Buf.append("\n");

	for (int new_size = Buf.size(); old_size < new_size; old_size++) {
		if (Buf[old_size] == '\n')
			LineOffsets.push_back(old_size + 1);
	}
}

void Logger::AddLog(int level, const char* file, int line, const char* fmt, ...) {
	int old_size = Buf.size();

	va_list args;
	va_start(args, fmt);
	Buf.appendf("%-5s %s:%d: ", log_level_string(level), file, line);
	Buf.appendfv(fmt, args);
	Buf.append("\n");
	va_end(args);

	for (int new_size = Buf.size(); old_size < new_size; old_size++) {
		if (Buf[old_size] == '\n')
			LineOffsets.push_back(old_size + 1);
	}
}

void Logger::Show() {
	Visible = true;
}

void Logger::Hide() {
	Visible = false;
}

bool Logger::IsHidden() {
	return !Visible;
}

void Logger::Draw(const char* title) {
	if (!Visible) return;

	if (!ImGui::Begin(title, NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
		ImGui::End();
		return;
	}

	ImVec2 WinSize = { 450, 300 };
	ImGui::SetWindowPos({ ImGui::GetIO().DisplaySize.x - WinSize.x - 5, 30 }, ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(WinSize, ImGuiCond_FirstUseEver);

	// Options menu
	if (ImGui::BeginPopup("Options")) {
		ImGui::Checkbox("Auto-scroll", &AutoScroll);
		ImGui::EndPopup();
	}

	// Main window
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");

	ImGui::SameLine();
	bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	Filter.Draw("Filter", -100.0f);

	ImGui::Separator();
	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (clear)
		Clear();
	if (copy)
		ImGui::LogToClipboard();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	const char* buf = Buf.begin();
	const char* buf_end = Buf.end();

	if (Filter.IsActive()) {
		for (int line_no = 0; line_no < LineOffsets.Size; line_no++) {
			const char* line_start = buf + LineOffsets[line_no];
			const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
			if (Filter.PassFilter(line_start, line_end))
				ImGui::TextUnformatted(line_start, line_end);
		}
	} else {
		ImGuiListClipper clipper;
		clipper.Begin(LineOffsets.Size);
		while (clipper.Step()) {
			for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
				const char* line_start = buf + LineOffsets[line_no];
				const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
				ImGui::TextUnformatted(line_start, line_end);
			}
		}
		clipper.End();
	}

	ImGui::PopStyleVar();

	if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
	ImGui::End();
}

extern "C" void Logger_AddLog(int level, const char* file, int line, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	GetLogger().AddLog(level, file, line, fmt, args);
	va_end(args);
}
