#include "console.h"
#include <stdint.h>
#include <string.h>
#include "intr.h"
#include "multiboot.h"
#include "task.h"
#include "memorymanager.h"
#include "serial.h"

void init(multiboot_info_t* mb_info, unsigned long magic)
{
    clear_screen();
    init_serial();
    //initalize the memory manager
    init_memory_manager(mb_info);
    
	
    //clean the multiboot info after it isn't needed anymore
    mb_info = NULL;

    kprintf("Initalizing GDT...\n");
    init_gdt();
    kprintf("Initalizing IDT...\n");
    init_idt();
    init_multitasking();

}
