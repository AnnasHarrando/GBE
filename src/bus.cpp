#include "bus.h"

uint8_t bus::read(uint16_t addr) {
        if (addr < 0x8000) {
            //ROM Data
            return emu_read(addr,CART);
        }
        else if (addr < 0xA000) {
            //Char/Map Data
            return emu_read(addr,VRAM);
        }
        else if (addr < 0xC000) {
            //Cartridge RAM
            return emu_read(addr,CART);
        }
        else if (addr < 0xE000) {
            //WRAM (Working RAM)
            return emu_read(addr,WRAM);
        }
        else if (addr < 0xFE00) {
            //reserved echo ram...
            return 0;
        }
        else if (addr < 0xFEA0) {
            //OAM
            return emu_read(addr,OAM);
        }
        else if (addr < 0xFF00) {
            //reserved unusable...
            return 0;
        }
        else if (addr < 0xFF80) {
            //IO Registers...
            return emu_read(addr,IO);
        }
        else if(addr < 0xFFFF){
            return emu_read(addr,HRAM);
        }
        else if(addr == 0xFFFF){
            //  return
            return get_ie_register();
        }
        printf("No bus addr %04X", addr);
        exit(-5);

}

void bus::write(uint16_t addr, uint8_t val) {
    //printf("Writing to %04X\n\n", addr);
    if (addr < 0x8000) {
        //ROM Data
        emu_write(addr, val,CART);
    }
    else if (addr < 0xA000) {
        //Char/Map Data
        emu_write(addr, val,VRAM);
    }
    else if (addr < 0xC000) {
        //EXT-RAM
        emu_write(addr, val,CART);
    }
    else if (addr < 0xE000) {
        //WRAM
        emu_write(addr, val,WRAM);
    }
    else if (addr < 0xFE00) {
        //reserved echo ram
    }
    else if (addr < 0xFEA0) {
        //OAM
        if(dma_active()) return;
        emu_write(addr, val,OAM);
    }
    else if (addr < 0xFF00) {
        //unusable reserved
    }
    else if (addr < 0xFF80) {
        //IO Registers...
        emu_write(addr, val,IO);
    }
    else if (addr < 0xFFFF) {
        emu_write(addr, val,HRAM);
    }
    else if(addr == 0xFFFF){
        //CPU SET ENABLE REGISTER
        set_ie_register(val);
    }
}
