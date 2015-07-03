#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "multiboot.h"
#include <stdint.h>

#define BLOCK_SIZE 4096

#define NULL ((void*) 0)

void init_memory_manager(multiboot_info_t* mb_info);
void free(void* page);
uint32_t alloc();

#endif
