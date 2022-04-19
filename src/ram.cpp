#include "ram.h"

uint8_t ram::wram_read(uint16_t addr){
    return wram[addr - 0xC000];
}

void ram::wram_write(uint16_t addr, uint8_t val){
    wram[addr - 0xC000] = val;
}

uint8_t ram::hram_read(uint16_t addr){
    return hram[addr - 0xC000];
}

void ram::hram_write(uint16_t addr, uint8_t val){
    hram[addr - 0xC000] = val;
}