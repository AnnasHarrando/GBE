#include "ram.h"

static uint8_t wram[0x2000];
static uint8_t hram[0x80];

uint8_t wram_read(uint16_t addr){
    addr -= 0xC000;
    return wram[addr];
}

void wram_write(uint16_t addr, uint8_t val){
    addr -= 0xC000;
    hram[addr] = val;
}

uint8_t hram_read(uint16_t addr){
    addr -= 0xC000;
    return wram[addr];
}

void hram_write(uint16_t addr, uint8_t val){
    addr -= 0xC000;
    hram[addr] = val;
}