#ifndef GBE_EMU_H
#define GBE_EMU_H

#include "SDL.h"
#include "screen.h"
#include <windows.h>
#include <thread>
#include <cstdio>

typedef enum {
    HRAM,
    WRAM,
    VRAM,
    RAM_BANK,
    OAM,
    CART,
    CART_BANK,
    IO
}component;

uint8_t emu_read(uint16_t addr, component comp);
void emu_write(uint16_t addr, uint8_t val, component comp);
uint8_t bus_read(uint16_t addr);
void bus_write(uint16_t addr, uint8_t val);
bool dma_active();
int start(int argc, char **argv);
uint8_t get_ie_register();
void set_ie_register(uint8_t val);
void get_interrupt(uint8_t val);
uint8_t get_int_flags();
void set_int_flags(uint8_t val);
void dma_start(uint8_t val);
void fifo_init();

uint8_t get_window_line();

void cycles(uint8_t cycle);

#endif //GBE_BUS_H


