#include "console.h"
#include "stdint.h"
#include "intr.h"
#include "multiboot.h"
#include "task.h"


void init(multiboot_info_t* mbd, unsigned long magic)
{
   

    clear_screen();
    //kprintf("%x\n",magic);
    kprintf("%x\n",mbd->mmap_length);
    if(mbd->flags & (1 << 6)) {
      memory_map_t* mmap = mbd->mmap_addr;
      while(mmap < mbd->mmap_addr + mbd->mmap_length) {
        kprintf("%x%x\n",mmap->base_addr_high, mmap->base_addr_low);
	mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int));
      }
    }

    kprintf("Initalizing GDT...\n");
    init_gdt();
    kprintf("Initalizing IDT...\n");
    //init_idt();
    


    //init_multitasking();
}
