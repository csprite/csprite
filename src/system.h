#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SYS_PATH_MAX_SIZE
	#define SYS_PATH_MAX_SIZE 4096
#endif

#if defined(_WIN32) || defined(WIN32)
	#define SYS_PATH_SEP '\\'
#else
	#define SYS_PATH_SEP '/'
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
	Function: Sys_GetConfigDir
	Returns:
		- Windows: %APPDATA%
		- Linux: $XDG_CONFIG_HOME or $HOME/.config
		- Mac: $HOME/Library/Application Support
	Note:
		- Returned Pointer doesn't need to be freed as it's allocated & de-allocated autmatically.
*/
char* Sys_GetConfigDir(void);
size_t Sys_GetFileSize(FILE* fp); // Get File Size From A FILE*
void Sys_MakeDirRecursive(const char* dir); // Create Directories Recursively
char* Sys_GetBasename(const char* path); // Get Basename from a path
int Sys_ListDirContents(const char* dirpath, int (*callback)(const char *dirpath, const char *name, void* data), void* data); // Data is a pointer that will be passed to the callback function
int Sys_IsRegularFile(const char* filePath);
int Sys_IsRegularDir(const char* dirPath);

#ifdef __cplusplus
}
#endif
