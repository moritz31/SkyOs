#include "ata.h"
#include "console.h"

#define first_bus 0x1F0

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ( "outb %0, %1" : : "a" (val), "Nd" (port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

void init_ata() {
	//select master
	outb(first_bus + 6,0x70);
	//floating value test
	if(inb(first_bus + 7) & 0xFF) {
		kprintf("No devices: Floating Value\n");
	}
	
	//responsive device?
	outb(first_bus + 6,0x70);
	kprintf("%x\n",inb(first_bus + 7));
	
}