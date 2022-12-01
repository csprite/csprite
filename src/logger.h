#include "log/log.h"

#ifdef __cplusplus

#include "imgui/imgui.h"

// A Singleton Logger Class

class Logger {
private:
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.
	bool                Visible;
	static Logger       s_Instance;

public:
	Logger();
	~Logger();

	void Clear();
	void Draw(const char* title);
	void Show();
	void Hide();
	bool IsHidden();
	void AddLog(int level, const char* file, int line, const char* fmt, ...);
	void AddLog(int level, const char* file, int line, const char* fmt, va_list args);
	static Logger& Log();
};

Logger& GetLogger();

// A Simple Wrapper Around Logger Class & Console Logger (log/log.h)

#define Logger_Hide() GetLogger().Hide()
#define Logger_Show() GetLogger().Show()
#define Logger_IsHidden() GetLogger().IsHidden()
#define Logger_Draw(title) GetLogger().Draw(title)

#define Logger_Trace(...) \
	GetLogger().AddLog(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Debug(...) \
	GetLogger().AddLog(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Info(...)  \
	GetLogger().AddLog(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Warn(...)  \
	GetLogger().AddLog(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Error(...) \
	GetLogger().AddLog(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Fatal(...) \
	GetLogger().AddLog(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__);

#else

// C API For The Logger

void Logger_AddLog(int level, const char* file, int line, const char* fmt, ...);

#define Logger_Trace(...) \
	Logger_AddLog(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Debug(...) \
	Logger_AddLog(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Info(...)  \
	Logger_AddLog(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Warn(...)  \
	Logger_AddLog(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Error(...) \
	Logger_AddLog(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__);

#define Logger_Fatal(...) \
	Logger_AddLog(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__); \
	log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__);

#endif // __cplusplus
