#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int SysListDir(const char *dir, int (*callback)(const char *dir, const char *name, void* data), void* data);
int SysMakeDir(const char *path);
char* SysGetPaletteDir();
char* SysGetThemesDir();
char* SysFnameFromPath(const char* path);
long int fsize(FILE* f);

#if defined(_WIN32) || defined(WIN32)
	#define SYS_PATH_SEP '\\'
#else
	#define SYS_PATH_SEP '/'
#endif

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_H
