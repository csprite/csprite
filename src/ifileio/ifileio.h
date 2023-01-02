#ifndef I_FILE_IO_H_
#define I_FILE_IO_H_ 1

#include <stdint.h>

typedef unsigned char uchar_t;

#ifdef __cplusplus
extern "C" {
#endif

// Full Support For Reading & Writing PNGs & JPEGs/JPGs
int ifio_write_uchar(const char* filePath, uchar_t* pixels, uint32_t w, uint32_t h);
uchar_t* ifio_read_uchar(const char* filePath, uint32_t* w_ptr, uint32_t* h_ptr);

#ifdef __cplusplus
}
#endif

#endif // I_FILE_IO_H_
