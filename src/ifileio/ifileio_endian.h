#ifndef _IFILEIO_ENDIAN_H
#define _IFILEIO_ENDIAN_H 1

#include <stdint.h>

// swap_xxxxxx functions simple swap bytes, can be usefully for converting between endianness
int16_t  swap_int16(int16_t val);
uint16_t swap_uint16(uint16_t val);

int32_t  swap_int32(int32_t val);
uint32_t swap_uint32(uint32_t val);

#if TARGET_IS_BIGENDIAN == 1
	#define SWAP_ONLY_BIGE_i32(val) swap_int32(val)
	#define SWAP_ONLY_BIGE_u32(val) swap_uint32(val)
	#define SWAP_ONLY_BIGE_i16(val) swap_int16(val)
	#define SWAP_ONLY_BIGE_u16(val) swap_uint16(val)
#else
	#define SWAP_ONLY_BIGE_i32(val) val
	#define SWAP_ONLY_BIGE_u32(val) val
	#define SWAP_ONLY_BIGE_i16(val) val
	#define SWAP_ONLY_BIGE_u16(val) val
#endif

#endif // _IFILEIO_ENDIAN_H

