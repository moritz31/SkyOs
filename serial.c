#include "serial.h"

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ( "outb %0, %1" : : "a" (val), "Nd" (port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );

    return ret;
}


void init_serial(uint32_t port) {
	outb(port + 1, 0x00);
	outb(port + 3, 0x80);
	outb(port + 0, 0x03);
	outb(port + 1, 0x00);
	outb(port + 3, 0x03);
	outb(port + 2, 0xC7);
	outb(port + 4, 0x0B);
}

int isWriteable() {
	//5th Bit AND 1 
	return inb(port + 5) & 0x20;
}

void writeData(char data) {
	while(isWriteable() == 0) {
		outb(port, a);
	}
}
