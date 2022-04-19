#include "SDL.h"
#include "screen.h"
#include <windows.h>
#include <thread>
#include <cstdio>


int start(int argc, char **argv);
uint8_t bus_read(uint16_t addr);
void bus_write(uint16_t addr, uint8_t val);

