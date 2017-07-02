#include "serial.h"

#define PORT COM1

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ( "outb %0, %1" : : "a" (val), "Nd" (port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );

    return ret;
}


void init_serial() {
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x80);
	outb(PORT + 0, 0x03);
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x03);
	outb(PORT + 2, 0xC7);
	outb(PORT + 4, 0x0B);
}

uint8_t get_lsb() {
	return inb(PORT + 5);
}

void writeCharacter(char data) {
	//kprintf("%d\n",(get_lsb() & (1 << 5)) != 0);
	if((get_lsb() & (1 << 5)) != 0) {
		outb(PORT, data);
	}
}

char readCharacter() {
	if(get_lsb() & 1) {
		return inb(PORT);
	}
}
