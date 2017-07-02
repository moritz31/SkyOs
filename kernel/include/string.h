#ifndef STRING_H
#define STRING_H

#define NULL 0

#include <stddef.h>
#include <stdint.h>

void* memset(void *s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);

#endif
