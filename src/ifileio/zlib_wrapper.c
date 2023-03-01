#include <zlib.h>
#include <stdlib.h>
#include "zlib_wrapper.h"

uint8_t* Z_CompressData(size_t dataToCompressSize, size_t* sizeDataCompressed, uint8_t* dataToCompress) {
	*sizeDataCompressed = (dataToCompressSize * 1.1) + 12;
	uint8_t* dataCompressed = (uint8_t*)malloc(*sizeDataCompressed);

	int32_t z_result = compress(
		dataCompressed, (uLongf*)sizeDataCompressed,
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

uint8_t* Z_DeCompressData(uint8_t* dataToDecompress, size_t dataToDecompressSize, size_t originalDataSize) {
	size_t sizeDataUncompressed = originalDataSize;
	uint8_t* dataUncompressed = (uint8_t*)malloc(sizeDataUncompressed);

	int32_t z_result = uncompress(
		dataUncompressed, (uLongf*)&sizeDataUncompressed,
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

