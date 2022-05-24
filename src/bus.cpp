#include "bus.h"
#include "emu.h"
#include "cart.h"

static cart *cart;


uint8_t bus::read(uint16_t addr) {
        if (addr < 0x4000) {
            return emu_read(addr,CART);
        }
        if (addr < 0x8000) {
            return emu_read(addr,CART_BANK);
        }
        else if (addr < 0xA000) {
            //if(oam_mode()) return 0xFF;
            return emu_read(addr,VRAM);
        }
        else if (addr < 0xC000) {
            return emu_read(addr,RAM_BANK);
        }
        else if (addr < 0xE000) {
            return emu_read(addr,WRAM);
        }
        else if (addr < 0xFE00) {
            return 0;
        }
        else if (addr < 0xFEA0) {
            if(dma_active()) {
                printf("test\n");
                return 0xFF;
            }
            else return emu_read(addr,OAM);
        }
        else if (addr < 0xFF00) {
            return 0;
        }
        else if (addr < 0xFF80) {
            return emu_read(addr,IO);
        }
        else if(addr < 0xFFFF){
            return emu_read(addr,HRAM);
        }
        else if(addr == 0xFFFF){
            return get_ie_register();
        }
        printf("No bus addr %04X", addr);
        exit(-5);

}

void bus::write(uint16_t addr, uint8_t val) {
    if (addr < 0x4000) {
        emu_write(addr, val,CART);
    }
    if (addr < 0x8000) {
        emu_write(addr, val, CART_BANK);
    }
    else if (addr < 0xA000) {
        //if(oam_mode()) return;
        emu_write(addr, val,VRAM);
    }
    else if (addr < 0xC000) {
        emu_write(addr, val,RAM_BANK);
    }
    else if (addr < 0xE000) {
        emu_write(addr, val,WRAM);
    }
    else if (addr < 0xFE00) {

    }
    else if (addr < 0xFEA0) {

        if(dma_active()) return;
        emu_write(addr, val,OAM);
    }
    else if (addr < 0xFF00) {

    }
    else if (addr < 0xFF80) {
        emu_write(addr, val,IO);
    }
    else if (addr < 0xFFFF) {
        emu_write(addr, val,HRAM);
    }
    else if(addr == 0xFFFF){
        set_ie_register(val);
    }
}
