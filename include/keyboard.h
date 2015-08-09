#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "stdint.h"

typedef enum {
	a = 0x1e,
	b = 0x30,
	c = 0x2e,
	d = 0x20,
	e = 0x12,
	f = 0x21,
	g = 0x22,
	h = 0x23,
	i = 0x17,
	j = 0x24,
	k = 0x25,
	l = 0x26,
	m = 0x32,
	n = 0x31,
	o = 0x18,
	p = 0x19,
	q = 0x10,
	r = 0x13,
	s = 0x1f,
	t = 0x14,
	u = 0x16,
	v = 0x2f,
	w = 0x11,
	x = 0x2d,
	y = 0x2c,
	z = 0x15,
	space = 0x39,
	enter = 0x1c
} key_code;

void init_keyboard();
void send_command(uint8_t command);
void keyboard_handler();
void test();


#endif