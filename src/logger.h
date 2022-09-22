#include "imgui/imgui.h"

class Logger {
private:
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.
	bool                Visible;

public:
	Logger();
	~Logger();

	void Clear();
	void Draw(const char* title);
	void Show();
	void Hide();
	void AddLog(int level, const char* file, int line, const char* fmt, ...);
};

// A Simple Wrapper Around Logger Class & Console Logger (log/log.h)

#define Logger_Trace(lgr, ...) \
	lgr.AddLog(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Debug(lgr, ...) \
	lgr.AddLog(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Info(lgr, ...)  \
	lgr.AddLog(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Warn(lgr, ...)  \
	lgr.AddLog(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Error(lgr, ...) \
	lgr.AddLog(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Fatal(lgr, ...) \
	lgr.AddLog(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__);
