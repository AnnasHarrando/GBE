#include "bus.h"
#include "cpu.h"


uint8_t bus_read(uint16_t addr) {
    if (addr < 0x8000) {
        //ROM Data
        return cart_read(addr);
    } 
    else if (addr < 0xA000) {
        //Char/Map Data
    }
    else if (addr < 0xC000) {
        //Cartridge RAM
        return cart_read(addr);
    }
    else if (addr < 0xE000) {
        //WRAM (Working RAM)
        return wram_read(addr);
    }
    else if (addr < 0xFE00) {
        //reserved echo ram...
        return 0;
    }
    else if (addr < 0xFEA0) {
        //OAM
        return 0;
    }
    else if (addr < 0xFF00) {
        //reserved unusable...
        return 0;
    }
    else if (addr < 0xFF80) {
        //IO Registers...
        return 0;
    }
    else if(addr < 0xFFFF){
        return hram_read(addr);
    }
    else if(addr == 0xFFFF){
      //  return
      return get_ie_register();
    }
    printf("No bus addr %04X", addr);
    exit(-5);
}

void bus_write(uint16_t addr, uint8_t val) {
    if (addr < 0x8000) {
        //ROM Data
        cart_write(addr, val);
    }
    else if (addr < 0xA000) {
        //Char/Map Data
    }
    else if (addr < 0xC000) {
        //EXT-RAM
        cart_write(addr, val);
    }
    else if (addr < 0xE000) {
        //WRAM
        wram_write(addr, val);
    }
    else if (addr < 0xFE00) {
        //reserved echo ram
    }
    else if (addr < 0xFEA0) {
        //OAM
    }
    else if (addr < 0xFF00) {
        //unusable reserved
    }
    else if (addr < 0xFF80) {
        //IO Registers...
    }
    else if (addr < 0xFFFF) {
        hram_write(addr, val);
    }
    else if(addr == 0xFFFF){
        //CPU SET ENABLE REGISTER
        set_ie_register(val);
    }
}