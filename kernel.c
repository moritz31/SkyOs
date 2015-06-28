#include "console.h"
#include "stdint.h"
#include "intr.h"

void init(void)
{
    
    clear_screen();
    
    kprintf("Initalizing GDT...\n");
    init_gdt();
    kprintf("Initalizing IDT...\n");
    init_idt();
}
