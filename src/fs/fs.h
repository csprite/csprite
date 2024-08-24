#ifndef SRC_FILESYSTE_FILESYSTEM_H_INCLUDED_
#define SRC_FILESYSTE_FILESYSTEM_H_INCLUDED_
#pragma once

typedef void* dir_t;

/*
 Description:
  Opens a directory and returns a handle
 */
dir_t FsListDirStart(const char* path);

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
int FsListDir(dir_t dir, char** name, int* isDir);

/*
 Description:
  Rewind directory stream to start
 */
void FsListDirRewind(dir_t dir);

/*
 Description:
  Close directory handle returned by `FsListDirStart(...)`
 */
void FsListDirEnd(dir_t dir);

/*
 Description:
  Get `cc` part of `c:/aa/bb/cc` path
 Notes:
  - Returns index at which basename starts in the `path` variable
  - Returns negative value on error
 */
int FsGetBasename(const char* path);

/*
 Description:
  Get `c:/aa/bb/` part of `c:/aa/bb/cc` path
 Notes:
  - Returns non-negative index that points to
    END of the path of parent dir
  - Values are set to negative if not found
 */
int FsGetParentDir(const char* path);

#endif
