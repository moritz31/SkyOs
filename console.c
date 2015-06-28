#include "console.h"

//position on the screen
static int x = 0;
static int y = 0;

//video adress
static char* video = (char*) 0xb8000;

//defines screen width and height 
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT= 25;

//kprintf return
static int kprintf_res = 0;

/*
The function clear the screen by deleting every character
*/
void clear_screen(void) {
	//loop through all characteres
	for(int i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT*2;i++) {
		//clear the characteres
		video[i] = 0;
	}
	//set pointer position to zero
	x = y = 0;
}

static void kputc(char c) {
	
	//reached line end
	if(c == '\n' || x > 79) {
		x = 0;
		y++;
	}
	//line break;
	if(c == '\n') {
		return;
	}

	//if y > 24 move one up and delete last line
	if(y > 24) {
		int i;
		for(i = 0;i < 2 * 24 * 80;i++) {
			video[i] = video[i + 160];
		}

		for(;i < 2 * 25 * 80;i++) {
			video[i] = 0;
		}
		y--;
	}
	//write the character
	video[2 * (y * 80 + x)] = c;
	video[2 * (y * 80 + x) + 1] = 0x07;

	//adjust position
	x++;
	kprintf_res++;

}

static void kputs(const char* s) {
	while(*s) {
		kputc(*s++);
	}
}

static void kputn(unsigned long x, int base) {
	char buf[65];
	const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* p;

	if(base > 36) {
		return;
	}
	
	p = buf + 64;
	*p = '\0';
	do {
		*--p = digits[x % base];
		x /= base;
	} while (x);
	kputs(p);
}

int kprintf(const char* fmt, ...) {
	va_list ap;
	const char* s;
	unsigned long n;
	
	va_start(ap, fmt);
	kprintf_res = 0;
	while(*fmt) {
		if(*fmt == '%') {
			fmt++;
			switch(*fmt) {
				case 'c':
					s = va_arg(ap, char*);
					kputc((char)s);
					break;
				case 's':
					s = va_arg(ap, char*);
					kputs(s);
					break;
				case 'd':
				case 'u':
					n = va_arg(ap, unsigned long int);
					kputn(n, 10);
					break;
				case 'x':
				case 'p':
					n = va_arg(ap, unsigned long int);
					kputn(n, 16);
					break;
				case '%':
					kputc('%');
					break;
				case '\0':
					goto out;
				default:
					kputc('%');
					kputc(*fmt);
					break;
			}
		} else {
			kputc(*fmt);	
		}

		fmt++;
	}

	out:
		va_end(ap);

		return kprintf_res;
}
