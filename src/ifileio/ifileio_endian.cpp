#include "ifileio/ifileio_endian.h"

i16 swap_val(i16 val) {
	return (val << 8) | ((val >> 8) & 0xFF);
}

u16 swap_val(u16 val) {
	return (val << 8) | (val >> 8);
}

i32 swap_val(i32 val) {
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF ); 
	return (val << 16) | ((val >> 16) & 0xFFFF);
}

u32 swap_val(u32 val) {
	val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
	return (val << 16) | (val >> 16);
}
