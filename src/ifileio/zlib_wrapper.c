#include <zlib.h>
#include <stdlib.h>
#include "zlib_wrapper.h"
#include "log/log.h"

uint8_t* Z_CompressData(uLongf dataToCompressSize, uLongf* sizeDataCompressed, uint8_t* dataToCompress) {
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
			log_error("ZLib Z_MEM_ERROR!");
			free(dataCompressed);
			return NULL;
			break;
		case Z_BUF_ERROR: // output buffer wasn't large enough
			log_error("ZLib Z_BUF_ERROR!");
			free(dataCompressed);
			return NULL;
			break;
		default: // some other error
			log_error("ZLib Un-Caught Error: %d", z_result);
			free(dataCompressed);
			return NULL;
			break;
	}

	return dataCompressed;
}

uint8_t* Z_DeCompressData(uint8_t* dataToDecompress, uLongf dataToDecompressSize, uLongf originalDataSize) {
	uLongf sizeDataUncompressed = originalDataSize;
	uint8_t* dataUncompressed = (uint8_t*)malloc(sizeDataUncompressed);

	int32_t z_result = uncompress(
		dataUncompressed, &sizeDataUncompressed,
		dataToDecompress, dataToDecompressSize
	);

	switch(z_result) {
		case Z_OK:
			break;
		case Z_MEM_ERROR: // out of memory
			log_error("ZLib Z_MEM_ERROR!");
			free(dataUncompressed);
			return NULL;
			break;
		case Z_BUF_ERROR: // output buffer wasn't large enough
			log_error("ZLib Z_BUF_ERROR!");
			free(dataUncompressed);
			return NULL;
			break;
		default: // some other error
			log_error("ZLib Un-Caught Error: %d", z_result);
			free(dataUncompressed);
			return NULL;
			break;
	}

	return dataUncompressed;
}

