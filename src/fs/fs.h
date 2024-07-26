#ifndef SRC_FILESYSTE_FILESYSTEM_H_INCLUDED_
#define SRC_FILESYSTE_FILESYSTEM_H_INCLUDED_
#pragma once

/*
 Description:
  Loads `dir` contents one by one into `*name` with each
  subsequent function call, where `*isDir` is set to non-zero
  to indicate if the entry is a dir.
 Notes:
  - Thread un-safe.
  - In first call `dir` shall be set, in subsequent calls `dir` should be NULL.
  - Memory pointed by `*name` is only valid till the next function call.
  - Returns non-zero value if an error occurs.
  - Sets `*name` to NULL to indicate end of contents.
 */
int fs_list_dir(const char* dir, char** name, int* isDir);

/*
 Description:
  Call this to prematurely end `fs_list_dir` without
  having to wait for NULL entry.
 Notes:
  - Thread un-safe
 */
void fs_list_dir_abrupt_end(void);

/*
 Description:
  Get `cc` part of `c:/aa/bb/cc` path
 Notes:
  - Returns index at which basename starts in the `path` variable
  - Returns negative value on error
 */
int fs_get_basename(const char* path);

#endif
