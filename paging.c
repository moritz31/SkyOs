#include "paging.h"
#include <stdint.h>
#include <string.h>

#define PTE_PRESENT 0x1
#define PTE_WRITE 0x2
#define PTE_USER 0x4

static struct virtual_context* kernel_context = NULL;
static struct virtual_context* current_context = NULL;
struct virtual_context* create_context() {
	struct virtual_context* context = alloc();
	context->pagedirectory = alloc();
	context = kernel_context;
	for(int i = 256; i < 1024; i++) {
		context->pagedirectory[i] = NULL;
	}

	return context;
}



void activate_context(struct virtual_context* context) {
	asm volatile("mov %0, %%cr3" : : "r" (context->pagedirectory));
}

int map_page (struct virtual_context* context, uint32_t phys, uint32_t virt, int flag) {
	
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

		if(flag) {
			context->pagedirectory[directory_index] = 
			(uint32_t) page_table | PTE_PRESENT | PTE_WRITE | PTE_USER;
		} else {
			context->pagedirectory[directory_index] = 
			(uint32_t) page_table | PTE_PRESENT | PTE_WRITE;
		}
		
	}
	
	
	if(flag) {
		page_table[table_index] = phys | PTE_PRESENT | PTE_WRITE | PTE_USER;
	} else {
		page_table[table_index] = phys | PTE_PRESENT | PTE_WRITE;
	}
	asm volatile("invlpg %0" : : "m" (*(char*)virt));

	return 0;

}

void init_paging() {
	uint32_t cr0;

	kernel_context = create_context();

	//map all used parts
	for(int i = 0; i < 31250; i++) {
		for(int j = 0; j < 32; j++) {
			if(~(get_bitmap(i) & (1 << j))) {
				map_page(kernel_context,(i*32+j)*4096,(i*32+j)*4096,0);
			}
		}
	}
	activate_context(kernel_context);

	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= (1 << 31);
	asm volatile("mov %0, %%cr0" : : "r" (cr0));
}

void set_current_context(struct virtual_context* context) {
	current_context = context;
}

uint32_t get_free_page() {
	for(int i = 256; i < 1024;i++) {
		if(current_context->pagedirectory[i] == NULL) {
			return (uint32_t)(i*4096*1024);
		} else {
			uint32_t* page_table = current_context->pagedirectory[i];
			for(int j = 0; j < 1024; j++) {
				if(page_table[j] == NULL) {
					return (uint32_t) (i*4096*1024+j);
				}
			}
		}
	}

	return NULL;
}

uint32_t virtual_alloc() {
	uint32_t* page = alloc();

	uint32_t virt_addr = get_free_page();
	kprintf("%x",virt_addr);
	map_page(current_context,page,virt_addr,1);

	return virt_addr;
	
}



