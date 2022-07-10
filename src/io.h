#ifndef GBE_IO_H
#define GBE_IO_H

#include <cstdint>
#include "timer.h"
#include "lcd.h"
#include "audio.h"


uint8_t io_read(uint16_t addr);
void io_write(uint16_t addr, uint8_t val);

void io_init();
#endif