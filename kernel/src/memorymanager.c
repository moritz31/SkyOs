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

block_t* head = &kernel_start;
block_t* current;

uint32_t* heap_start;
uint32_t* heap_current;

mem_t memory_map[10];
uint32_t mmap_cntr = 0;

static void mark_used(void *page);

block_t* createNewBlock(enum TYPE type, uint32_t start, uint32_t length, block_t* next) {

	block_t* newBlock = heap_current;
	heap_current += 0x1000;
	newBlock->mem_type = type;
	newBlock->start_addr = start;
	newBlock->length = length;
	newBlock->next_block = next;

	kprintf("[Create] new: %x head: %x\n",newBlock->start_addr,head->start_addr);
	return newBlock;
}

void insertBlock(block_t *newBlock) {

	kprintf("new: %x head: %x\n",newBlock->start_addr,head->start_addr);
	// We need to insert before head		
	if(head->start_addr >= newBlock->start_addr) {

		head->start_addr = newBlock->start_addr + newBlock->length + 1;

		newBlock->next_block = head;
		head = newBlock;
		return;
	}

	for(block_t* c = head; c->next_block != NULL; c = c->next_block) {
		if(c->next_block->start_addr > newBlock->start_addr) {
			c->start_addr = newBlock->start_addr + newBlock->length + 1;
			newBlock->next_block = c->next_block;
			c->next_block = newBlock;
		}
	}
}

/**
 * @brief Initalize the memory manager
 * @author Moritz Weichert
 * @param mb_info Pointer to multiboot structure
 */
void init_memory_manager(multiboot_info_t *mb_info)
{
	kprintf("Initalizing memory ... \n");

	heap_start = &kernel_end + 0x1000;
	heap_current = heap_start;

	// Initale block
	head->mem_type   = PROCESS;
	head->start_addr = 0x0;
	head->length     = BLOCKS*BLOCK_SIZE;
	head->next_block = NULL;
	current = head;
	//get start and end of the mmap
	memory_map_t *mmap = (void *)mb_info->mmap_addr;
	memory_map_t *mmap_end = (void *)mb_info->mmap_addr + mb_info->mmap_length;

	//set BITMAP to 0 all blocks are full
	//memset(BITMAP, 0, sizeof(BITMAP));

	//aslong as mmap isn't at the end
	while (mmap < mmap_end)
	{
		//if adresse is free
		if (mmap->type == 1)
		{
			//get start and end of free block
			void *addr_start = (void*)mmap->base_addr_low;
			void *addr_end = (void*)addr_start + mmap->length_low;
			//kprintf("%d %x %x\n",mmap_cntr,addr_start,addr_end);
			/*block_t* new_block = (block_t*)createNewBlock(HOLE,addr_start,addr_end - addr_start, NULL);

			insertBlock(new_block);*/
			memory_map[mmap_cntr].start_addr = addr_start;
			memory_map[mmap_cntr].end_addr = addr_end;
			memory_map[mmap_cntr].mem_type = HOLE;

			//set all free pages to 1
			/*while (addr_start < addr_end)
			{
				free((void *)addr_start);
				addr_start += 0x1000;
			}*/
			//kprintf("Start: %x End: %x\n",addr_start, addr_end);
			mmap_cntr++;
		}
		//get next mmap
		
		mmap++;
	}

	int len = 0;
	for(int i = 0; i < mmap_cntr; i++) {
		if(memory_map[i].end_addr + 1 != memory_map[i+1].start_addr) {
			//We need to add a new entry
			memory_map[i+2] = memory_map[i+1];
			memory_map[i+1].start_addr = memory_map[i].end_addr + 1;
			memory_map[i+1].end_addr   = memory_map[i+2].start_addr - 1;
			memory_map[i+1].mem_type   = PROCESS;
			len++;
		}
		len++;
	}

	mem_t kernel = { .start_addr = &kernel_start, .end_addr = &kernel_end, .mem_type = PROCESS };
	memory_map[len] = kernel;

	if(memory_map[len-1].start_addr == memory_map[len].start_addr) {
		mem_t tmp = memory_map[len-1];
		memory_map[len-1] = memory_map[len];
		memory_map[len] = tmp;
		memory_map[len].start_addr = memory_map[len-1].end_addr +1 ;
	}

	for(int i = 0; i < len+1; i++) {
		kprintf("%x %x %d\n",memory_map[i].start_addr,memory_map[i].end_addr,memory_map[i].mem_type);
	}

	/*for(block_t* c = head; c != NULL; c = c->next_block) {
		kprintf("[Block] Type: %d Start: %x Length: %x Next: %x \n",c->mem_type,c->start_addr,c->length,c->next_block);
	}*/

	//kprintf("Start: %x End: %x\n", &kernel_start, &kernel_end);
	//kernel must be set blocked seperatly
	/*uint32_t kernel_addr = (uint32_t)&kernel_start;
	while ((void *)kernel_addr < &kernel_end)
	{
		mark_used((void *)kernel_addr);
		kernel_addr += 0x1000;
	}*/
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
