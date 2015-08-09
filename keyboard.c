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

void init_keyboard() {
	while(inb(0x64) & 0x1) {
		inb(0x60);
	}
	
	send_command(0xF0);
	waitForResponse();
	send_command(0x02);
	waitForResponse();
	send_command(0xF4);
	waitForResponse();
}

void send_command(uint8_t command) {
	while(inb(0x64) & 0x2) {}
	outb(0x60,command);	
}

void waitForResponse() {
	while (!(inb(0x64) & 0x1)) {}	
	uint8_t response = inb(0x60);
	kprintf("%x",response);
}

char decodeScanCode(uint8_t scan_code) {
	char key_code;
	switch(scan_code) {
		case a: 
			key_code = 'a';
			break;
		case b:
			key_code = 'b';
			break;
		case c:
			key_code = 'c';
			break;
		case d: 
			key_code = 'd';
			break;
		case e:
			key_code = 'e';
			break;
		case f:
			key_code = 'f';
			break;
		case g: 
			key_code = 'g';
			break;
		case h:
			key_code = 'h';
			break;
		case i:
			key_code = 'i';
			break;
		case j: 
			key_code = 'j';
			break;
		case k:
			key_code = 'k';
			break;
		case l:
			key_code = 'l';
			break;
		case m: 
			key_code = 'm';
			break;
		case n:
			key_code = 'n';
			break;
		case o:
			key_code = 'o';
			break;
		case p: 
			key_code = 'p';
			break;
		case q:
			key_code = 'q';
			break;
		case r:
			key_code = 'r';
			break;
		case s: 
			key_code = 's';
			break;
		case t:
			key_code = 't';
			break;
		case u:
			key_code = 'u';
			break;
		case v: 
			key_code = 'v';
			break;
		case w:
			key_code = 'w';
			break;
		case x:
			key_code = 'x';
			break;
		case y: 
			key_code = 'y';
			break;
		case z:
			key_code = 'z';
			break;
		case space:
			key_code = ' ';
			break;
		case enter:
			key_code = '\n';
			break;
		default: 
			return 0;
	}
	
	return key_code;
}

void keyboard_handler() {
	uint8_t scan_code;
	if(inb(0x64) & 0x1) {
		scan_code = inb(0x60);
	}
	
	//handle key up key down
	if(scan_code & (1 << 7)) {
		// key released
	} else {
		//key pressed
		char key = decodeScanCode(scan_code);
		kputc(key);
	}
	
}