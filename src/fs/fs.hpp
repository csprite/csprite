#ifndef CSP_FILESYSTEM_FILESYSTEM_HPP_INCLUDED_
#define CSP_FILESYSTEM_FILESYSTEM_HPP_INCLUDED_
#pragma once

#include "types.hpp"
#include <functional>

#ifdef TARGET_WINDOWS
	#define SYS_PATH_SEP "\\"
	#define SYS_PATH_SEP_CHAR '\\'
#else
	#define SYS_PATH_SEP "/"
	#define SYS_PATH_SEP_CHAR '/'
#endif

namespace Fs {
	String GetConfigDir();
	inline String GetLanguagesDir() { return Fs::GetConfigDir() + SYS_PATH_SEP "languages"; }
	inline String GetPalettesDir() { return Fs::GetConfigDir() + SYS_PATH_SEP "palettes"; }

	String GetParentDir(const String& path);
	String GetBaseName(const String& path);

	int MakeDir(const char* const path);
	inline int MakeDir(const String& path) { return Fs::MakeDir(path.c_str()); }
	int MakeDirRecursive(const String& path);

	// returns -1 on error, errno is set
	i32 GetFileSize(const String& filePath);

	// returns 1 if regular, 0 if not, -1 on error, with errno set
	i32 IsRegularFile(const String& filePath);
	i32 IsRegularDir(const String& dirPath);

	// if callback function returns false, the function doesn't continue listing
	typedef std::function<bool(const char* path, bool isFile)> OnListDirCB;
	// returns number of items listed, -1 is returned on error & errno is set
	i32 ListDir(const char* dirPath, OnListDirCB cb);
}

#endif // CSP_FILESYSTEM_FILESYSTEM_HPP_INCLUDED_

