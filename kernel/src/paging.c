#include "paging.h"

#include "memorymanager.h"

context *kernel_context;

context *current_context;

/**
 * 
 */
void create_context( context *c );

/**
 * 
 */
void map_page(void *phys, void *virt, uint32_t flags);

void init_paging( void ) {

    kprintf("Initalizing Paging ...");

    

    create_context(kernel_context);

    //Mapped used bytes
    for(int i = 0x10000; i < 31250; i++) {
        uint32_t bitmap = get_bitmap(i);
        for(int j = 0; j < 32; j++) {
            if(bitmap & ~(1<<j)) {
                uint32_t page = (i*32+j)*4096;
                map_page(page,page,0x3);
            }
        }
    }

    asm volatile("mov %%eax, %%cr3": :"a"(kernel_context->page_directory));	
	asm volatile("mov %cr0, %eax");
	asm volatile("orl $0x80000000, %eax");
	asm volatile("mov %eax, %cr0");

    kprintf("[OK]\n");
}

void create_context(context* c) {
    c = (void*)kalloc(sizeof(context));

    for(int i = 0; i < 1024; i++) {
        c->page_directory[i] = 0x2;
    }

    uint32_t* first_pt __attribute__((aligned(4096))) = (uint32_t*)kalloc(sizeof(uint32_t));

    for(int i = 0 ; i < 1024; i++) {
        first_pt[i] = (i* 0x1000) | 3;
    }

    c->page_directory[0] = (uint32_t)first_pt | 3;

    c->page_directory[1023] = (uint32_t)c->page_directory | 3;
}

void map_page(void *phys, void *virt, uint32_t flags) {

    uint32_t pdindex = (uint32_t)virt >> 22;
    uint32_t ptindex = (uint32_t)virt >> 12 & 0x3FF;

    uint32_t *pd = (uint32_t *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
 
    uint32_t *pt = ((uint32_t *)0xFFC00000) + (0x1000 * pdindex);
    // Here you need to check whether the PT entry is present.

    if(pd[pdindex] & 1) {
        //Page table exists
        
        if(!pt[ptindex] & 1) {
            pt[ptindex] = (uint32_t)phys | flags;
        }
    } else {
        uint32_t* new_page_table = (uint32_t*)kalloc(sizeof(uint32_t));
        for(int i = 0; i < 0x400; i++) {
            new_page_table[i] = 0x0;
        }

        pd[pdindex] = (uint32_t)new_page_table | 0x3;
        pt[ptindex] = (uint32_t)phys | flags;

    }

    asm volatile("invlpg %0" : : "m" (virt));

}

void *vmem_alloc() {

    void *phys = kalloc(sizeof(uint32_t));

    map_page(phys,phys,0x3);

    return phys;
}