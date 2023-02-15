#ifndef _ZLIB_WRAPPER_H
#define _ZLIB_WRAPPER_H 1

#include <stdint.h>
#include <stddef.h>

uint8_t* Z_CompressData(size_t dataToCompressSize, size_t* sizeDataCompressed, uint8_t* dataToCompress);
uint8_t* Z_DeCompressData(uint8_t* dataToDecompress, size_t dataToDecompressSize, size_t originalDataSize);

#endif // _ZLIB_WRAPPER_H

