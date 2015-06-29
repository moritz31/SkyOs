#include "console.h"
#include "stdint.h"
#include "intr.h"
#include "multiboot.h"
#include "task.h"


void init(multiboot_info_t* mbd, unsigned long magic)
{
   

    clear_screen();
    kprintf("%x\n",mbd->mmap_addr);
    kprintf("%x\n",mbd->mmap_length);
    if(mbd->flags & (1 << 6)) {
      memory_map_t* mmap = (void*)mbd->mmap_addr;
      kprintf("%d + %d\n",mbd->mmap_addr, mbd->mmap_length);
      while((void*)mmap < mbd->mmap_addr + mbd->mmap_length) {
        kprintf("0x%x - 0x%x | %d\n",mmap->base_addr_low, mmap->length_low,mmap->type);
	mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int));
      }
    }

    kprintf("Initalizing GDT...\n");
    init_gdt();
    kprintf("Initalizing IDT...\n");
    //init_idt();
    


    //init_multitasking();
}
