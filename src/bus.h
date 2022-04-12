#include "cart.h"
#include "ram.h"
#include <cstdio>
#include <cstdlib>

uint8_t bus_read(uint16_t addr);
void bus_write(uint16_t addr, uint8_t val);
