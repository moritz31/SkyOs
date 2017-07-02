#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "multiboot.h"
#include <stdint.h>
#include "console.h"

#define BLOCK_SIZE 4096

void init_memory_manager(multiboot_info_t* mb_info);
void free(void* page);
uint32_t alloc();

#endif
