#include "paging.h"
#include <stdint.h>
#include <string.h>

#define PTE_PRESENT 0x1
#define PTE_WRITE 0x2

struct virtual_context* create_context() {
	struct virtual_context* context = alloc();
	context->pagedirectory = alloc();
	for(int i = 0; i < 1024; i++) {
		context->pagedirectory[i] = NULL;
	}

	return context;
}



void activate_context(struct virtual_context* context) {
	asm volatile("mov %0, %%cr3" : : "r" (context->pagedirectory));
}

int map_page (struct virtual_context* context, uint32_t phys, uint32_t virt) {
	
	uint32_t* page_table;
	uint32_t pagenumber = virt / 4096;
	uint32_t directory_index = pagenumber / 1024;
	uint32_t table_index = pagenumber % 1024;
	if ((virt & 0xFFF) || (phys & 0xFFF)) {
		return -1;
	}

	if(context->pagedirectory[directory_index] & PTE_PRESENT) {
		page_table = (uint32_t*) (context->pagedirectory[directory_index] & ~0xFFF);
	} else {
		page_table = alloc();
		for(int i = 0; i < 1024; i++) {
			page_table[i] = NULL;
		}
		context->pagedirectory[directory_index] = (uint32_t) page_table | PTE_PRESENT | PTE_WRITE;
	}
	
	
	page_table[table_index] = phys | PTE_PRESENT | PTE_WRITE;
	asm volatile("invlpg %0" : : "m" (*(char*)virt));

	return 0;

}

static struct virtual_context* kernel_context;

void init_paging() {
	uint32_t cr0;

	kernel_context = create_context();

	//map all used parts
	for(int i = 0; i < 4096*1024; i++) {
		map_page(kernel_context, i, i);
	}
		

	activate_context(kernel_context);

	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= (1 << 31);
	asm volatile("mov %0, %%cr0" : : "r" (cr0));
}

uint32_t virtual_alloc() {
	uint32_t* page = alloc();
}

