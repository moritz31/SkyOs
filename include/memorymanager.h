#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "multiboot.h"
#include <stdint.h>

#define BLOCK_SIZE 4096

struct stack {
	uint32_t  addr;
	

} __attribute__((packed)) stack_t;

void init_memory_manager(multiboot_info_t* mb_info);
void push_stack(uint32_t addr);
void pop_stack(uint32_t addr);

#endif
