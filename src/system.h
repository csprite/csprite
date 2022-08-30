#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdio.h>

int sys_list_dir(const char *dir, int (*callback)(const char *dir, const char *name, void* data), void* data);
int sys_make_dir(const char *path);
long int fsize(FILE* f);

#if defined(_WIN32) || defined(WIN32)
	#define SYS_PATH_SEP '\\'
#else
	#define SYS_PATH_SEP '/'
#endif

#endif // SYSTEM_H
