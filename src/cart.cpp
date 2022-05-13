#include "cart.h"

uint8_t cart::read(uint16_t addr) {
    return rom_data[addr];
}

void cart::write(uint16_t addr, uint8_t val) {
    if(addr < 0x2000){

    }
    else if(addr < 0x4000){
        if(val == 0) val = 1;
        val &= 0b11111;
        cur_rom_bank = rom_data + (0x4000*val);
    }
}

uint8_t cart::rom_bank_read(uint16_t addr) {
    return cur_rom_bank[addr - 0x4000];
}

void cart::rom_bank_write(uint16_t addr, uint8_t val) {

}

uint8_t cart::ram_read(uint16_t addr) {
    return 0;
}

void cart::ram_write(uint16_t addr, uint8_t val) {

}

