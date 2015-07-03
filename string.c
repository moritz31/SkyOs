#include <string.h>

void* memset(void *s, int c, uint32_t n) {

	unsigned char* p = s;
	while(n--) {
		*p++ = (unsigned char) c;
	}
	return s;
}
