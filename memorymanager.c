#include "memorymanager.h"

void init_memory_manager(multiboot_info_t* mb_info) {

	//check if the mmap flag is valid to retrive the mmap
	if(mb_info->flags & (1 << 6)) {
		//initalize an mmap struture
		memory_map_t* mmap = (void*) mb_info->mmap_addr;

		/*
		overjump the first free block and loop until the next free block
		*/
		do {
			//get the next mmap entry
			mmap = (memory_map_t*) ((uint32_t)mmap + mmap->size + sizeof(uint32_t));
		} while(mmap->type != 1);

		//start and end of the free adress space
		uint32_t base_addr_start = mmap->base_addr_low;
		uint32_t base_addr_end = (mmap->base_addr_low + mmap->length_low);
		
		while(base_addr_end > base_addr_start) {
			
			//allocate the memory in 4k blocks;
			base_addr_end -= BLOCK_SIZE;
		} 
		kprintf("%d : %d\n",base_addr_start,base_addr_end);

	} else {
		//TODO can't get memory map
	}

}
