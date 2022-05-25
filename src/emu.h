#ifndef GBE_EMU_H
#define GBE_EMU_H

#include "SDL.h"
#include "cpu.h"
#include "cart.h"
#include "ram.h"
#include "io.h"
#include "ppu.h"
#include "bus.h"
#include "ppu_fifo.h"
#include "audio.h"
#include "screen.h"
#include "lcd.h"
#include <windows.h>
#include <cstdio>


int start(char **argv);
void cycles(uint8_t cycle);
uint8_t bus_read(uint16_t addr);

cart *get_cart();
io *get_io();
ram *get_ram();
ppu *get_ppu();
cpu *get_cpu();
bus *get_bus();
timer *get_timer();
lcd *get_lcd();
ppu_fifo *get_ppu_fifo();


#endif //GBE_BUS_H


