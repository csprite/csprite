#ifndef _ZLIB_WRAPPER_H
#define _ZLIB_WRAPPER_H 1

#include <zlib.h>
#include <stdint.h>
typedef unsigned long ulong_t;

uint8_t* _CompressData(ulong_t dataToCompressSize, ulong_t* sizeDataCompressed, uint8_t* dataToCompress) {
	*sizeDataCompressed = (dataToCompressSize * 1.1) + 12;
	uint8_t* dataCompressed = (uint8_t*)malloc(*sizeDataCompressed);

	int32_t z_result = compress(
		dataCompressed, sizeDataCompressed,
		dataToCompress, dataToCompressSize
	);

	switch(z_result) {
		case Z_OK:
			break;
		case Z_MEM_ERROR: // out of memory
			return NULL;
			break;
		case Z_BUF_ERROR: // output buffer wasn't large enough
			return NULL;
			break;
		default: // some other error
			return NULL;
			break;
	}

	return dataCompressed;
}

uint8_t* _DeCompressData(uint8_t* dataToDecompress, ulong_t dataToDecompressSize, ulong_t originalDataSize) {
	ulong_t sizeDataUncompressed = originalDataSize;
	uint8_t* dataUncompressed = (uint8_t*)malloc(sizeDataUncompressed);

	int32_t z_result = uncompress(
		dataUncompressed, &sizeDataUncompressed,
		dataToDecompress, dataToDecompressSize
	);

	switch(z_result) {
		case Z_OK:
			break;
		case Z_MEM_ERROR: // out of memory
			return NULL;
			break;
		case Z_BUF_ERROR: // output buffer wasn't large enough
			return NULL;
			break;
		default: // some other error
			return NULL;
			break;
	}

	return dataUncompressed;
}

#endif // _ZLIB_WRAPPER_H

