#ifdef TARGET_WINDOWS
	#include "windows.h"
#endif

void EnableVT100() {
#ifdef TARGET_WINDOWS
	u32 iMode = 0;
	int iHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (iHandle == INVALID_HANDLE_VALUE || iHandle == NULL) return;
	GetConsoleMode(iHandle, &iMode);
	SetConsoleMode(iHandle, iMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	u32 oMode = 0;
	int oHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (oHandle == INVALID_HANDLE_VALUE || oHandle == NULL) return;
	GetConsoleMode(oHandle, &oMode);
	SetConsoleMode(oHandle, oMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

