#ifndef _ZLIB_WRAPPER_H
#define _ZLIB_WRAPPER_H 1

#include <stdint.h>
#include <stddef.h>

uint8_t* Z_CompressData(uLongf dataToCompressSize, uLongf* sizeDataCompressed, uint8_t* dataToCompress);
uint8_t* Z_DeCompressData(uint8_t* dataToDecompress, uLongf dataToDecompressSize, uLongf originalDataSize);

#endif // _ZLIB_WRAPPER_H

