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
uint64_t get_timing();

cart *get_cart();
ram *get_ram();
ppu *get_ppu();
cpu *get_cpu();
timer *get_timer();
lcd *get_lcd();
ppu_fifo *get_ppu_fifo();
audio *get_audio();

SDL_AudioDeviceID get_dev();


#endif //GBE_BUS_H


