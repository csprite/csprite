/**
 * Copyright (c) 2017 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `sfd.c` for details.
 */

#ifndef SFD_H
#define SFD_H
#pragma once

#define SFD_VERSION "0.1.0"

typedef struct {
  const char* title;       // Title that will appear in dialog
  const char* path;        // Default path to open the dialog in
  const char* filter_name; // Name of filter that will appear in dialog
  const char* filter;      // File Filter in Format: "*.ext1|*.ext2"
  const char* extension;   // Default Extension
  int save;                // Is Save File Dialog? (save == 1)
} sfd_Options;

#ifdef __cplusplus
extern "C" {
#endif

const char* sfd_get_error(void);
const char* sfd_open_dialog(sfd_Options* opt);

#ifdef __cplusplus
}
#endif

#endif // SFD_H
