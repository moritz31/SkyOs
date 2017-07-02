#include "stdint.h"
#include "stdarg.h"

void clear_screen(void);
void kputc(char c);
void kdelc();
void kprintf(const char* fmt, ...);

