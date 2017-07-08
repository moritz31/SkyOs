#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"

#define BLOCK_SIZE 4096

void init_memory_manager(multiboot_info_t* mb_info);
void free(void* page);
void *alloc( size_t size );

#endif
