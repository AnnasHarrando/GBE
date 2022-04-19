#include <cstdint>
#include "emu.h"


void tick();
uint8_t timer_read(uint16_t address);
void timer_write(uint16_t addr, uint8_t val);