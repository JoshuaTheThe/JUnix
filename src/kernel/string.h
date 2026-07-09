
#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void memset(void *p, int v, long l);
int strncmp(const char *const Lhs, const char *const Rhs, unsigned long Len);
int strcmp(const char *const Lhs, const char *const Rhs);
int strnlen(const char *const A, unsigned long Len);
int strlen(const char *const A);
void memcpy(void *Destination, const void *const Source, const unsigned long Len);
char *UlToString(unsigned long Number);
int itoa(char *dest, int n, int base, bool uppercase);
void strncpy(void *Destination, const void *const Source, const unsigned long Len);
void dump(void *buf, size_t n);

#endif
