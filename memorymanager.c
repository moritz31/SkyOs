#include "memorymanager.h"
#include "string.h"
//4GB memory to handle in blocks of 4kb
//4GB/4kb = 1Million entrys 
//every BITMAP entry has 32 bits so 1M/ 32 = 31250
#define BLOCKS 31250
static uint32_t BITMAP[BLOCKS];

extern void* kernel_start;
extern void* kernel_end;

static void mark_used(void* page);

void init_memory_manager(multiboot_info_t* mb_info) {

	kprintf("Initalizing memory ... \n");
	//get start and end of the mmap
	memory_map_t* mmap = (void*) mb_info->mmap_addr;
	memory_map_t* mmap_end = (void*) mb_info->mmap_addr + mb_info->mmap_length;
	
	//set BITMAP to 0 all blocks are full
	memset(BITMAP, 0, sizeof(BITMAP));
	//aslong as mmap isn't at the end
	while(mmap < mmap_end) {
		//if adresse is free
		if(mmap->type == 1) {
			//get start and end of free block
			uint32_t addr_start = mmap->base_addr_low;
			uint32_t addr_end = addr_start + mmap->length_low;

			//set all free pages to 1
			while(addr_start < addr_end) {
				free((void*) addr_start);
				addr_start += 4096;
			}
		}


		//kernel must be set blocked seperatly
		uint32_t kernel_addr = (uint32_t)kernel_start;
		while((void*)kernel_addr < kernel_end) {
			mark_used((void*)kernel_addr);
			kernel_addr += 4096;
		}
		//get next mmap
		mmap++;
	} 
}

/*
	Function to set allocated a specific page
*/
static void mark_used(void* page) {
	uint32_t index = (uint32_t) page / 4096;
	BITMAP[index / 32] &= ~(1 << (index % 32));
}

/*
	Function to free an specific page
*/
void free(void* page) {
	uint32_t index = (uint32_t) page / 4096;
	BITMAP[index / 32] |=  1 << (index % 32); 
}

/*
	Function to allocate the next free page
*/
uint32_t alloc() {	

	//loop through the BITMAP looking for a free block (1)
	for(int i = 0; i < BLOCKS; i++) {
		for(int j = 0; j < 32; j++) {
			if(BITMAP[i] & (1 << j)) {
				//set the freeblock used and return the adress of it 
				BITMAP[i] &= ~(1 << j);
				return (i * 32 + j) * 4096;
			}
		}
	}
	//no page found
	return NULL;
}
