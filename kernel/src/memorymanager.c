#include "memorymanager.h"

#include "string.h"
#include "console.h"
//4GB memory to handle in blocks of 4kb
//4GB/4kb = 1Million entrys
//every BITMAP entry has 32 bits so 1M/ 32 = 31250

#define BLOCKS 31250
static uint32_t BITMAP[BLOCKS];

extern const void kernel_start;
extern const void kernel_end;

static void mark_used(void *page);

/**
 * @brief Initalize the memory manager
 * @author Moritz Weichert
 * @param mb_info Pointer to multiboot structure
 */
void init_memory_manager(multiboot_info_t *mb_info)
{

	kprintf("Initalizing memory ... \n");

	//get start and end of the mmap
	memory_map_t *mmap = (void *)mb_info->mmap_addr;
	memory_map_t *mmap_end = (void *)mb_info->mmap_addr + mb_info->mmap_length;

	//set BITMAP to 0 all blocks are full
	memset(BITMAP, 0, sizeof(BITMAP));

	//aslong as mmap isn't at the end
	while (mmap < mmap_end)
	{
		//if adresse is free
		if (mmap->type == 1)
		{
			//get start and end of free block
			void *addr_start = (void*)mmap->base_addr_low;
			void *addr_end = (void*)addr_start + mmap->length_low;

			//set all free pages to 1
			while (addr_start < addr_end)
			{
				free((void *)addr_start);
				addr_start += 0x1000;
			}
		}
		//get next mmap
		mmap++;
	}

	//kernel must be set blocked seperatly
	uint32_t kernel_addr = (uint32_t)&kernel_start;
	while ((void *)kernel_addr < &kernel_end)
	{
		mark_used((void *)kernel_addr);
		kernel_addr += 0x1000;
	}
}

/**
 * @brief Mark a given block as used
 * @author Moritz Weichert
 * @param page Pointer to block which should be marked as used
 */
static void mark_used(void *page)
{
	uint32_t index = (uint32_t)page / 4096;
	BITMAP[index / 32] &= ~(1 << (index % 32));
}

/**
 * @brief Set a used block to free
 * @author Moritz Weichert
 * @param page Pointer to block which should be marked as free
 */
void free(void *page)
{
	uint32_t index = (uint32_t)page / 4096;
	BITMAP[index / 32] |= 1 << (index % 32);
}

/**
 * @brief Allocates a given amount of memory
 * @author Moritz Weichert
 * @param size Size needed in bytes
 * @return Pointer to allocated address
 */
void *kalloc( size_t size)
{

	//loop through the BITMAP looking for a free block (1)
	for (int i = 0; i < BLOCKS; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			// Try to find free block
			if (BITMAP[i] & (1 << j))
			{
				//set the freeblock used and return the adress of it
				BITMAP[i] &= ~(1 << j);
				return (void*)((i * 32 + j) * 4096);
			}
		}
	}

	//no page found
	return NULL;
}

/**
 * @brief Returns a requested row of the bitmap
 * @author Moritz Weichert
 * @param i Row which should be returned
 * @return Row of the bitmap
 */
uint32_t get_bitmap(int i)
{
	return BITMAP[i];
}
