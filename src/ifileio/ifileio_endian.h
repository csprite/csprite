#ifndef _IFILEIO_ENDIAN_H
#define _IFILEIO_ENDIAN_H 1

#include <stdint.h>

// macro requires C99 compound literals support
#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})

// swap_xxxxxx functions simple swap bytes, can be usefully for converting between endianness
int16_t  swap_int16(int16_t val);
uint16_t swap_uint16(uint16_t val);

int32_t  swap_int32(int32_t val);
uint32_t swap_uint32(uint32_t val);

// only swap if on big-endian system
#define SWAP_ONLY_BIGE_i32(val) (IS_BIG_ENDIAN ? swap_int32(val) : val)
#define SWAP_ONLY_BIGE_u32(val) (IS_BIG_ENDIAN ? swap_uint32(val) : val)
#define SWAP_ONLY_BIGE_i16(val) (IS_BIG_ENDIAN ? swap_int16(val) : val)
#define SWAP_ONLY_BIGE_u16(val) (IS_BIG_ENDIAN ? swap_uint16(val) : val)

#endif // _IFILEIO_ENDIAN_H

