#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

void init_serial();
void writeData(char data);
void writeCharacter(char data);
char readData();


#endif
