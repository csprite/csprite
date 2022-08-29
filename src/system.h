#ifndef SYSTEM_H
#define SYSTEM_H

int sys_list_dir(const char *dir, int (*callback)(const char *dir, const char *name, void* data), void* data);
int sys_make_dir(const char *path);
long int fsize(FILE* f);

#endif // SYSTEM_H
