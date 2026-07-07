
#ifndef _LIBK_H
#define _LIBK_H

#include <interrupts/sys.h>
#include <panic.h>

void exit(int code);
int write(int fd, const void *buf, size_t len);
int read(int fd, void *buf, size_t len);
int open(char *path, int flags);
void close(int fd);

#endif
