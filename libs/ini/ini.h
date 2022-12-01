/**
 * Copyright (c) 2016 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `ini.c` for details.
 */

#ifndef INI_H
#define INI_H

#ifdef __cplusplus
extern "C" {
#endif

#define INI_VERSION "0.1.1"

typedef struct ini_t ini_t;

ini_t*      ini_load(const char *filename);
ini_t*      ini_load_txt(const char *iniTxt);
void        ini_free(ini_t *ini);
const char* ini_get(ini_t *ini, const char *section, const char *key);
int         ini_sget(ini_t *ini, const char *section, const char *key, const char *scanfmt, void *dst);

#ifdef __cplusplus
}
#endif

#endif
