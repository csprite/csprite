#ifndef CSP_FILESYSTEM_FILESYSTEM_HPP_INCLUDED_
#define CSP_FILESYSTEM_FILESYSTEM_HPP_INCLUDED_
#pragma once

#include "types.hpp"

#ifdef TARGET_WINDOWS
	#define SYS_PATH_SEP "\\"
#else
	#define SYS_PATH_SEP "/"
#endif

namespace Fs {
	String GetConfigDir();
	inline String GetLanguageDir() { return Fs::GetConfigDir() + SYS_PATH_SEP "languages"; }

	String GetParentDir(const String& path);
	String GetBaseName(const String& path);

	// returns -1 on error, errno is set
	i32 GetFileSize(const String& filePath);

	// returns 1 if regular, 0 if not, -1 on error, with errno set
	i32 IsRegularFile(const String& filePath);
	i32 IsRegularDir(const String& dirPath);
}

#endif // CSP_FILESYSTEM_FILESYSTEM_HPP_INCLUDED_

