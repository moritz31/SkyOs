#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include "memorymanager.h"

struct virtual_context {
	uint32_t* pagedirectory;
};

void init_paging();
struct virtual_context* create_context();



#endif
