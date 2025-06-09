#ifndef SRC_FILESYSTE_FILESYSTEM_H_INCLUDED_
#define SRC_FILESYSTE_FILESYSTEM_H_INCLUDED_
#pragma once

typedef void* dir_t;

/*
 Description:
  Opens a directory and returns a handle
 */
dir_t fs_list_dir_start(const char* path);

/*
 Description:
  Loads `dir` contents one by one into `*name` with each
  subsequent function call, where `*isDir` is set to non-zero
  to indicate if the entry is a dir.
 Notes:
  - Memory pointed by `*name` is only valid till the next function call.
  - Returns non-zero value on error (Handle is closed automatically).
  - Sets `*name` to NULL to indicate end of contents.
 */
int fs_list_dir(dir_t dir, char** name, int* isDir);

/*
 Description:
  Rewind directory stream to start
 */
void fs_list_dirRewind(dir_t dir);

/*
 Description:
  Close directory handle returned by `fs_list_dir_start(...)`
 */
void fs_list_dirEnd(dir_t dir);

#endif
