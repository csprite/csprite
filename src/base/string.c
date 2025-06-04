#include "base/string.h"

String8 str8(U8* str, U64 size) {
	String8 s;
	s.str = str;
	s.size = size;
	return s;
}
