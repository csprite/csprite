#ifndef CSP_IFILEIO_IFILEIO_ENDIAN_HPP_INCLUDED_
#define CSP_IFILEIO_IFILEIO_ENDIAN_HPP_INCLUDED_ 1
#pragma once

#include "types.hpp"

// swap_xxxxxx functions simple swap bytes, can be usefully for converting between endianness
i16 swap_val(i16 val);
u16 swap_val(u16 val);
i32 swap_val(i32 val);
u32 swap_val(u32 val);

#if TARGET_IS_BIGENDIAN == 1
	#define SWAP_ONLY_BIGE(val) swap_val(val)
#else
	#define SWAP_ONLY_BIGE(val) val
#endif

#endif // CSP_IFILEIO_IFILEIO_ENDIAN_HPP_INCLUDED_
