#include "cart.h"

uint8_t cart::read(uint16_t addr) {
    return rom_data[addr];
}

void cart::write(uint16_t addr, uint8_t val) {
    if(addr < 0x2000){
        if(val == 0x0A) ram_enabled = true;
        else ram_enabled = false;
    }
    else if(addr < 0x4000){
        //printf("val: %04X\n",val);
        if(val == 0) val = 1;
        //val &= 0b11111;
        cur_rom_bank = rom_data + (0x4000*val);
    }
}

uint8_t cart::rom_bank_read(uint16_t addr) {
    return cur_rom_bank[addr - 0x4000];
}

void cart::rom_bank_write(uint16_t addr, uint8_t val) {
    if(addr < 0x6000){
        cur_ram_bank_val = val & 0b11;
        if(ram_bank_enabled) {
            cur_ram_bank = ram_banks + (0x2000 * cur_ram_bank_val);
            //printf("ram changed: %i\n",cur_ram_bank_val);
            //if(battery_enabled) battery_save();
        }
    }
    else if(addr < 0x8000){
        ram_bank_enabled = val & 1;

        if(ram_bank_enabled) {
            cur_ram_bank = ram_banks + (0x2000 * cur_ram_bank_val);
            //if(battery_enabled) battery_save();
        }
    }
}

uint8_t cart::ram_read(uint16_t addr) {
    if(!ram_enabled) return 0xFF;
    return cur_ram_bank[addr - 0xA000];
}

void cart::ram_write(uint16_t addr, uint8_t val) {
    if(ram_enabled) {
        cur_ram_bank[addr - 0xA000] = val;
        //if(battery) battery_enabled = true;
    }
}

