#include "keyboard.h"
#include "console.h"

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ( "outb %0, %1" : : "a" (val), "Nd" (port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

void test() {
	if(~(inb(0x64) && (1 << 1)) && (inb(0x64) && 0x2)) {
		outb(0x64,0xAA);
		kprintf("%x",inb(0x60));
	}
}

void init_keyboard() {
	while(inb(0x64) & 0x1) {
		inb(0x60);
	}
	
	send_command(0xF4);
	
	send_command(0xF0);
	send_command(0x02);
	
	send_command(0xF0);
	send_command(0x00);
	while(inb(0x64) & 0x1) {
		kprintf("%x",inb(0x60));
	}
	
}

void send_command(uint8_t command) {
	while(inb(0x64) & 0x2) {
		outb(0x64,command);	
	}
}

void send_data(uint8_t data) {
	while(inb(0x64) & 0x2) {
		outb(0x60, data);
	}
}

void keyboard_handler() {
	uint8_t scan_code;
	uint8_t key;
	if(inb(0x64) & 0x1) {
		scan_code = inb(0x60);
	}
	
	kprintf("%x",scan_code);
}