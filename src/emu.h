#include "SDL.h"
#include "screen.h"
#include <windows.h>
#include <thread>
#include <cstdio>


int start(int argc, char **argv);
uint8_t bus_read(uint16_t addr);
void bus_write(uint16_t addr, uint8_t val);
void get_interrupt(uint8_t val);
uint8_t get_int_flags();
void set_int_flags(uint8_t val);
void dma_start(uint8_t val);

void cycles(uint8_t cycle);

