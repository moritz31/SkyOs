#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "stdint.h"

void init_keyboard();
void send_command(uint8_t command);
void keyboard_handler();
void test();


#endif