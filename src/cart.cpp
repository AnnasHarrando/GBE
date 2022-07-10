#include "cart.h"


uint8_t cart::read(uint16_t addr) const {
    return rom_data[addr];
}

void cart::write(uint16_t addr, uint8_t val) {
    if(addr < 0x2000){
        if(val == 0x0A) ram_enabled = true;
        else ram_enabled = false;
    }
    else if(addr < 0x4000){
        // MBC1
        if(cart_type >= 1 && cart_type <= 3){
            if(val == 0) val = 1;

            if(rom_size > 0x40000) val &= 0b11111;
            else val &= 0b1111;
            cur_rom_bank = rom_data + (0x4000*val);
        }
        // MBC3
        if(cart_type >= 0xF && cart_type <= 0x13){
            if(val == 0) val = 1;
            val &= 0b1111111;
            cur_rom_bank = rom_data + (0x4000*val);
        }
        // MBC5
        if(cart_type >= 0x19 && cart_type <= 0x1E){
            if(addr < 0x2FFF) rom_bank_index = (rom_bank_index & 0x100) | val;
            else rom_bank_index = (rom_bank_index & 0xFF) | ((val & 0x1) << 8);
            cur_rom_bank = rom_data + (0x4000*rom_bank_index);
        }

    }
}

uint8_t cart::rom_bank_read(uint16_t addr) {
    return cur_rom_bank[addr - 0x4000];
}

void cart::rom_bank_write(uint16_t addr, uint8_t val) {
    if(addr < 0x6000){

        cur_ram_bank_val = val & 0b11;
        if (ram_bank_enabled) {
            cur_ram_bank = ram_banks + (0x2000 * cur_ram_bank_val);
        }
    }
    else if(addr < 0x8000){
            ram_bank_enabled = val & 1;
    }
}

uint8_t cart::ram_read(uint16_t addr) {
    if(!ram_enabled) return 0xFF;
    return cur_ram_bank[addr - 0xA000];
}

void cart::ram_write(uint16_t addr, uint8_t val) {

    if(ram_enabled && ram_size > 0) {
        // MBC1
        if(cart_type >= 1 && cart_type <= 3) {
            if (ram_bank_enabled) cur_ram_bank[addr - 0xA000] = val;
            else ram_banks[addr - 0xA000] = val;
        }

        cur_ram_bank[addr - 0xA000] = val;
    }
}

