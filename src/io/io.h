#ifndef IO_H
#define IO_H

unsigned char insb(unsigned short port);  // in byte
unsigned short insw(unsigned short port); // in word

void outb(unsigned short port, unsigned char val);  // out byte
void outw(unsigned short port, unsigned short val); // out word

#endif