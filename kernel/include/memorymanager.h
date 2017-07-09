#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"

#define BLOCK_SIZE 4096

void init_memory_manager(multiboot_info_t* mb_info);
void free(void* page);
void *alloc( size_t size );

enum TYPE {
    PROCESS,
    HOLE
};

typedef struct block_t block_t;

struct block_t {
    enum TYPE mem_type;
    uint32_t start_addr;
    size_t length;
    block_t* next_block;
};

typedef struct {
    uint32_t start_addr;
    uint32_t end_addr;
    enum TYPE mem_type;
} mem_t;

#endif
