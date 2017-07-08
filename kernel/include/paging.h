#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

typedef struct {
    uint32_t page_directory[1024];
} context __attribute__((aligned(4096)));

/**
 * 
 */
void init_paging( void );

/**
 * 
 */
void *vmem_alloc();



#endif
