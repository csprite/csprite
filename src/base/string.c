#include "base/string.h"
#include <string.h>

String8 str8(U8* str, U64 size) {
	String8 s;
	s.str = str;
	s.size = size;
	return s;
}

String8 str8_cstr(const char* str) {
	String8 s;
	s.str = (U8*)str;
	s.size = strlen(str);
	return s;
}
