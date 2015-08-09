#include "console.h"
#include "serial.h"

//position on the screen
static int x = 0;
static int y = 0;

//video adress
static char* video = (char*) 0xb8000;

//defines screen width and height 
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT= 25;

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ( "outb %0, %1" : : "a" (val), "Nd" (port));
}

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

void kputc(char c)
{
    if ((c == '\n') || (x > 79)) {
        x = 0;
        y++;
    }

    writeCharacter(c);

    if (c == '\n') {
        return;
    }

    if (y > 24) {
        int i;
        for (i = 0; i < 2 * 24 * 80; i++) {
            video[i] = video[i + 160];
        }

        for (; i < 2 * 25 * 80; i++) {
            video[i] = 0;
        }
        y--;
    }

    video[2 * (y * 80 + x)] = c;
    video[2 * (y * 80 + x) + 1] = 0x07;

    x++;
}

static void kputs(const char* s) {

	while(*s) {
		kputc(*s++);
	}
}

static void kputn(unsigned long int x, int base)
{
    char buf[65];
    const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* p;

    if (base > 36) {
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

void kprintf(const char* fmt, ...)
{
    va_list ap;
    const char* s;
    unsigned long n;

    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's':
                    s = va_arg(ap, char*);
                    kputs(s);
                    break;
		case 'b':
		    n = va_arg(ap, unsigned long int);
		    kputn(n, 2);
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
}
