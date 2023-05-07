#ifndef _ZLIB_WRAPPER_H
#define _ZLIB_WRAPPER_H 1
#pragma once

#include <stdint.h>
#include <stddef.h>

#include "zlib/zlib.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t* Z_CompressData(uLongf dataToCompressSize, uLongf* sizeDataCompressed, uint8_t* dataToCompress);
uint8_t* Z_DeCompressData(uint8_t* dataToDecompress, uLongf dataToDecompressSize, uLongf originalDataSize);

#ifdef __cplusplus
}
#endif

#endif // _ZLIB_WRAPPER_H
