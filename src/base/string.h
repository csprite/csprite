#ifndef SRC_BASE_STRING_H_INCLUDED_
#define SRC_BASE_STRING_H_INCLUDED_ 1
#pragma once

#include "base/types.h"

// Note: These strings are NOT null terminated!
typedef struct {
	U8* str;
	U64 size;
} String8;

#define str8_lit(s) (str8((U8*)(s), sizeof(s) - 1))

String8 str8(U8* str, U64 size);

#endif // SRC_BASE_STRING_H_INCLUDED_
