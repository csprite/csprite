#ifndef SRC_PLATFORM_PLATFORM_H_INCLUDED_
#define SRC_PLATFORM_PLATFORM_H_INCLUDED_ 1
#pragma once

#include "base/types.h"

void Platform_OpenURL(const char* URL);
void Platform_Abort(U32 ExitCode);
void Platform_AbortMessage(U32 ExitCode, const char* Message);

#endif // SRC_PLATFORM_PLATFORM_H_INCLUDED_
