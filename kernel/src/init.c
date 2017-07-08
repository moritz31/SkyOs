#include "console.h"
#include <stdint.h>
#include <string.h>
#include "intr.h"
#include "multiboot.h"
#include "task.h"
#include "memorymanager.h"
#include "serial.h"
#include "keyboard.h"
#include "paging.h"

void init(multiboot_info_t* mb_info, unsigned long magic)
{
    clear_screen();
    init_serial();

    //initalize the memory manager
    init_memory_manager(mb_info);
    init_paging();
    //clean the multiboot info after it isn't needed anymore
    mb_info = NULL;

    kprintf("Initalizing GDT... ");
    init_gdt();
    kprintf("[OK]\n");
    kprintf("Initalizing IDT... ");
    init_idt();
    kprintf("[OK]\n");

    /*for(int i = 0; i < 31; i++) {
        void* addr = vmem_alloc();
        kprintf("Virtual addr: %x\n",addr);
    }*/


    
    //init_keyboard();

    
    init_multitasking();
    //test();
}