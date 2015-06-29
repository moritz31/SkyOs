#include "console.h"
#include "stdint.h"
#include "intr.h"
#include "multiboot.h"
#include "task.h"


void init(multiboot_info_t* mbd, unsigned int magic)
{
    
    kprintf("%d",magic);
    clear_screen();

    kprintf("Initalizing GDT...\n");
    init_gdt();
    kprintf("Initalizing IDT...\n");
    init_idt();
    init_multitasking();
}
